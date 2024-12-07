#include <PySysLinkBase/SimulationModel.h>
#include <PySysLinkBase/ModelParser.h>
#include <PySysLinkBase/ISimulationBlock.h>
#include <PySysLinkBase/PortLink.h>
#include <PySysLinkBase/IBlockFactory.h>
#include <PySysLinkBase/BlockTypeSupportPlugingLoader.h>
#include <iostream>
#include <map>

int main() {
    std::unique_ptr<PySysLinkBase::BlockTypeSupportPlugingLoader> plugingLoader = std::make_unique<PySysLinkBase::BlockTypeSupportPlugingLoader>();

    std::map<std::string, std::unique_ptr<PySysLinkBase::IBlockFactory>> blockFactories = plugingLoader->LoadPlugins("/usr/local/lib");
    

    std::unique_ptr<PySysLinkBase::SimulationModel> simulationModel = std::make_unique<PySysLinkBase::SimulationModel>(PySysLinkBase::ModelParser::ParseFromYaml("/home/pello/PySysLink/Tests/system1.yaml", blockFactories));

    std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
    
    std::cout << "Block chains: " << std::endl;
    for (int i = 0; i < blockChains.size(); i++)
    {
        std::cout << "Block chain " << i << ":" << std::endl;
        for (int j = 0; j < blockChains[i].size(); j++)
        {
            std::cout << blockChains[i][j]->GetId() << std::endl;
        }
    }

    std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
    std::cout << "Ordered blocks: " << std::endl;
    for (int i = 0; i < orderedBlocks.size(); i++)
    {
        std::cout << orderedBlocks[i]->GetId() << std::endl;
        std::cout << orderedBlocks[i]->GetSampleTimes().size() << std::endl;
        std::cout << orderedBlocks[i]->GetSampleTimes()[0].GetSampleTimeType() << std::endl;
    }

    simulationModel->PropagateSampleTimes();
    std::cout << "Ordered blocks: " << std::endl;
    for (int i = 0; i < orderedBlocks.size(); i++)
    {
        std::cout << orderedBlocks[i]->GetId() << std::endl;
        std::cout << orderedBlocks[i]->GetSampleTimes().size() << std::endl;
        std::cout << orderedBlocks[i]->GetSampleTimes()[0].GetSampleTimeType() << std::endl;
    }

    return 0;
}

