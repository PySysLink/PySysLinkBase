#include "PySysLinkBase/SimulationModel.h"
#include "PySysLinkBase/ModelParser.h"
#include "PySysLinkBase/ISimulationBlock.h"
#include "PySysLinkBase/PortLink.h"
#include "PySysLinkBase/IBlockFactory.h"
#include "PySysLinkBase/BlockTypeSupportPlugingLoader.h"
#include "PySysLinkBase/SimulationManager.h"
#include "PySysLinkBase/SpdlogManager.h"
#include "PySysLinkBase/BlockEventsHandler.h"
#include <map>

int main() {

    PySysLinkBase::SpdlogManager::ConfigureDefaultLogger();
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::info);
    
    std::shared_ptr<PySysLinkBase::IBlockEventsHandler> blockEventsHandler = std::make_shared<PySysLinkBase::BlockEventsHandler>();

    std::unique_ptr<PySysLinkBase::BlockTypeSupportPlugingLoader> plugingLoader = std::make_unique<PySysLinkBase::BlockTypeSupportPlugingLoader>();
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::map<std::string, std::unique_ptr<PySysLinkBase::IBlockFactory>> blockFactories = plugingLoader->LoadPlugins("/usr/local/lib");
    
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::info);

    std::shared_ptr<PySysLinkBase::SimulationModel> simulationModel = std::make_unique<PySysLinkBase::SimulationModel>(PySysLinkBase::ModelParser::ParseFromYaml("/home/pello/PySysLink/Tests/system1.yaml", blockFactories, blockEventsHandler));

    std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
    

    std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
  

    simulationModel->PropagateSampleTimes();
    
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::unique_ptr<PySysLinkBase::SimulationManager> simulationManager = std::make_unique<PySysLinkBase::SimulationManager>();
    simulationManager->RunSimulation(simulationModel, 0.0, 10.0);

    return 0;
}

