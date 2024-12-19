#include "SimulationManager.h"
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{

    void SimulationManager::RunSimulation(std::shared_ptr<SimulationModel> simulationModel)
    {
        std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();

        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
        
        std::shared_ptr<SampleTime> constantSampleTime = std::make_unique<SampleTime>(SampleTimeType::constant);

        std::vector<std::shared_ptr<SampleTime>> discreteSampleTimes = {};
        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime = {};
        std::shared_ptr<SampleTime> currentSampleTime = std::make_unique<SampleTime>(SampleTimeType::discrete, 1.0);

        std::vector<std::shared_ptr<ISimulationBlock>> blocksWithConstantSampleTime = {};
        for (const auto& block : orderedBlocks)
        {
            if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::discrete)
            {
                bool isAlreadyOnDiscreteSampleTimes = false;
                for (const auto& discreteSampleTime : discreteSampleTimes)
                {
                    if (discreteSampleTime->GetDiscreteSampleTime() == block->GetSampleTime()->GetDiscreteSampleTime())
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
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::constant)
            {
                blocksWithConstantSampleTime.push_back(block);
            }
        }

        spdlog::get("default_pysyslink")->debug("Simulation start");
        for (auto& block : blocksWithConstantSampleTime)
        {
            spdlog::get("default_pysyslink")->debug("Processing block: {}", block->GetId());
            block->ComputeOutputsOfBlock(constantSampleTime);
            for (int i = 0; i < block->GetOutputPorts().size(); i++)
            {
                spdlog::get("default_pysyslink")->debug("Output port processing...");

                for (auto& connectedPort : simulationModel->GetConnectedPorts(block, i))
                {
                    block->GetOutputPorts()[i]->TryCopyValueToPort(*connectedPort);
                }
            }
        }

        std::vector<double> timeHits = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        for (const auto& timeHit : timeHits)
        {
            for (auto& block : blocksForEachDiscreteSampleTime[currentSampleTime])
            {
                spdlog::get("default_pysyslink")->debug("Processing block: ", block->GetId());
                block->ComputeOutputsOfBlock(constantSampleTime);
                for (int i = 0; i < block->GetOutputPorts().size(); i++)
                {
                    spdlog::get("default_pysyslink")->debug("Output port processing...");

                    for (auto& connectedPort : simulationModel->GetConnectedPorts(block, i))
                    {
                        block->GetOutputPorts()[i]->TryCopyValueToPort(*connectedPort);
                    }
                }
            }
        }
    }
}