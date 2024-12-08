#include "SimulationManager.h"
#include <iostream>

namespace PySysLinkBase
{

    void SimulationManager::RunSimulation(std::shared_ptr<SimulationModel> simulationModel)
    {
        std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();

        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
        
        std::unique_ptr<SampleTime> constantSampleTime = std::make_unique<SampleTime>(SampleTimeType::constant);

        std::vector<std::shared_ptr<SampleTime>> discreteSampleTimes = {};
        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime = {};
        std::shared_ptr<SampleTime> currentSampleTime = std::make_unique<SampleTime>(SampleTimeType::discrete, 1.0);

        std::vector<std::shared_ptr<ISimulationBlock>> blocksWithConstantSampleTime = {};
        for (const auto& block : orderedBlocks)
        {
            std::vector<SampleTime>& sampleTimesOnBlock = block->GetSampleTimes();
            for (const auto& sampleTimeOnBlock : sampleTimesOnBlock)
            {
                if (sampleTimeOnBlock.GetSampleTimeType() == SampleTimeType::discrete)
                {
                    bool isAlreadyOnDiscreteSampleTimes = false;
                    for (const auto& discreteSampleTime : discreteSampleTimes)
                    {
                        if (discreteSampleTime->GetDiscreteSampleTime() == sampleTimeOnBlock.GetDiscreteSampleTime())
                        {
                            isAlreadyOnDiscreteSampleTimes = true;
                        }
                    }
                    // std::shared_ptr<SampleTime> currentSampleTime = std::make_unique<SampleTime>(SampleTimeType::discrete, sampleTimeOnBlock.GetDiscreteSampleTime());

                    if (!isAlreadyOnDiscreteSampleTimes)
                    {
                        discreteSampleTimes.push_back(currentSampleTime);
                        blocksForEachDiscreteSampleTime.insert({currentSampleTime, std::vector<std::shared_ptr<ISimulationBlock>>({block})});
                    }
                    else
                    {
                        blocksForEachDiscreteSampleTime[currentSampleTime].push_back(block);
                    }
                }
                else if (sampleTimeOnBlock.GetSampleTimeType() == SampleTimeType::constant)
                {
                    blocksWithConstantSampleTime.push_back(block);
                }
            }
        }

        std::cout << "Simulation start" << std::endl;
        for (auto& block : blocksWithConstantSampleTime)
        {
            std::cout << "Processing block: " << block->GetId() << std::endl;
            block->ComputeOutputsOfBlock(*constantSampleTime);
            for (auto& outputPort : block->GetOutputPorts())
            {
                std::cout << "Output port processing..." << std::endl;

                for (auto& connectedPort : simulationModel->GetConnectedPorts(outputPort))
                {
                    std::cout << "Try copy to port: " << connectedPort->GetOwnerBlock().GetId() << std::endl;
                    outputPort->TryCopyValueToPort(*connectedPort);
                }
            }
        }

        std::vector<double> timeHits = {0, 1, 2, 3, 4, 5, 6, 7};
        for (const auto& timeHit : timeHits)
        {
            for (auto& block : blocksForEachDiscreteSampleTime[currentSampleTime])
            {
                std::cout << "Processing block: " << block->GetId() << std::endl;
                block->ComputeOutputsOfBlock(*constantSampleTime);
                for (auto& outputPort : block->GetOutputPorts())
                {
                    std::cout << "Output port processing..." << std::endl;

                    for (auto& connectedPort : simulationModel->GetConnectedPorts(outputPort))
                    {
                        std::cout << "Try copy to port: " << connectedPort->GetOwnerBlock().GetId() << std::endl;
                        outputPort->TryCopyValueToPort(*connectedPort);
                    }
                }
            }
        }
    }
}