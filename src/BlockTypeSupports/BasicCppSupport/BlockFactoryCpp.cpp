#include "BlockFactoryCpp.h"
#include <stdexcept>
#include <BlockLibraries/BasicBlocks/Constant.h>
#include <BlockLibraries/BasicBlocks/Sumator.h>
#include <BlockLibraries/BasicBlocks/Display.h>
#include "SimulationBlockCpp.h"
#include <iostream>


namespace BlockTypeSupports::BasicCppSupport
{
    BlockFactoryCpp::BlockFactoryCpp()
    {

    }

    // BlockFactoryCpp::~BlockFactoryCpp()
    // {

    // }

    std::unique_ptr<PySysLinkBase::ISimulationBlock> BlockFactoryCpp::CreateBlock(std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration)
    {
        std::string blockTypeId = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("BlockTypeId", blockConfiguration);

        std::cout << blockTypeId << " type block to create..." << std::endl;
        
        if (blockTypeId == "BasicBlocks/Constant")
        {
            std::unique_ptr<BlockTypes::BasicCpp::SimulationBlock> simulationBlock = std::make_unique<BlockLibraries::BasicBlocks::Constant>(blockConfiguration);
            return std::make_unique<SimulationBlockCpp>(std::move(simulationBlock), blockConfiguration);
        }
        else if (blockTypeId == "BasicBlocks/Sumator")
        {
            std::unique_ptr<BlockTypes::BasicCpp::SimulationBlock> simulationBlock = std::make_unique<BlockLibraries::BasicBlocks::Sumator>(blockConfiguration);
            return std::make_unique<SimulationBlockCpp>(std::move(simulationBlock), blockConfiguration);
        }
        else if (blockTypeId == "BasicBlocks/Display")
        {
            std::unique_ptr<BlockTypes::BasicCpp::SimulationBlock> simulationBlock = std::make_unique<BlockLibraries::BasicBlocks::Display>(blockConfiguration);
            return std::make_unique<SimulationBlockCpp>(std::move(simulationBlock), blockConfiguration);
        }
        else
        {
            throw std::out_of_range("Block type with id: " + blockTypeId + " not found in module BasicCpp.");
        }
    }

} // namespace BlockTypeSupports::BasicCppSupport
