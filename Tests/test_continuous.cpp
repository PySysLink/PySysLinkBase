#include "PySysLinkBase/SimulationModel.h"
#include "PySysLinkBase/ModelParser.h"
#include "PySysLinkBase/ISimulationBlock.h"
#include "PySysLinkBase/PortLink.h"
#include "PySysLinkBase/IBlockFactory.h"
#include "PySysLinkBase/BlockTypeSupportPlugingLoader.h"
#include "PySysLinkBase/SimulationManager.h"
#include "PySysLinkBase/SpdlogManager.h"
#include "PySysLinkBase/BlockEventsHandler.h"
#include "PySysLinkBase/SimulationOptions.h"
#include "PySysLinkBase/SimulationOutput.h"
#include <map>
#include <iostream>

int main() {

    PySysLinkBase::SpdlogManager::ConfigureDefaultLogger();
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);
    
    std::shared_ptr<PySysLinkBase::IBlockEventsHandler> blockEventsHandler = std::make_shared<PySysLinkBase::BlockEventsHandler>();

    std::unique_ptr<PySysLinkBase::BlockTypeSupportPlugingLoader> plugingLoader = std::make_unique<PySysLinkBase::BlockTypeSupportPlugingLoader>();
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::map<std::string, std::unique_ptr<PySysLinkBase::IBlockFactory>> blockFactories = plugingLoader->LoadPlugins("/usr/local/lib");
    
    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::shared_ptr<PySysLinkBase::SimulationModel> simulationModel = std::make_unique<PySysLinkBase::SimulationModel>(PySysLinkBase::ModelParser::ParseFromYaml("/home/pello/PySysLink/Tests/system1_continuous.yaml", blockFactories, blockEventsHandler));

    std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
    

    std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
  

    simulationModel->PropagateSampleTimes();

    PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::debug);

    std::shared_ptr<PySysLinkBase::SimulationOptions> simulationOptions = std::make_shared<PySysLinkBase::SimulationOptions>();
    simulationOptions->startTime = 0.0;
    simulationOptions->stopTime = 10.0;
    simulationOptions->runInNaturalTime = true;
    simulationOptions->naturalTimeSpeedMultiplier = 1;
    simulationOptions->blockIdsAndOutputIndexesToLog = {{"const1", 0}, {"integrator1", 0}};

    std::unique_ptr<PySysLinkBase::SimulationManager> simulationManager = std::make_unique<PySysLinkBase::SimulationManager>(simulationModel, simulationOptions);


    std::shared_ptr<PySysLinkBase::SimulationOutput> simulationOutput = simulationManager->RunSimulation();
    
    std::vector<double> continuousValues = simulationOutput->signals["Displays"]["display1"]->TryCastToTyped<double>()->values;
    std::vector<double> continuousTimes = simulationOutput->signals["Displays"]["display1"]->TryCastToTyped<double>()->times;
    for (int i = 0; i < continuousValues.size(); i++)
    {
        std::cout << continuousTimes[i] << ": " << continuousValues[i] << std::endl;
    }
    
    std::vector<double> continuousValuesLog = simulationOutput->signals["LoggedSignals"]["integrator1/0"]->TryCastToTyped<double>()->values;
    std::vector<double> continuousTimesLog = simulationOutput->signals["LoggedSignals"]["integrator1/0"]->TryCastToTyped<double>()->times;
    for (int i = 0; i < continuousValuesLog.size(); i++)
    {
        std::cout << continuousTimesLog[i] << ": " << continuousValuesLog[i] << std::endl;
    }

    return 0;
}

