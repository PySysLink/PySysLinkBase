#include "SimulationManager.h"
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{

    void SimulationManager::RunSimulation(std::shared_ptr<SimulationModel> simulationModel, double startTime, double endTime)
    {
        std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();

        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
        
        std::shared_ptr<SampleTime> constantSampleTime = std::make_unique<SampleTime>(SampleTimeType::constant);

        std::vector<std::shared_ptr<SampleTime>> discreteSampleTimes = {};
        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime = {};

        std::vector<std::shared_ptr<ISimulationBlock>> blocksWithConstantSampleTime = {};
        for (const auto& block : orderedBlocks)
        {
            if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::discrete)
            {
                bool isAlreadyOnDiscreteSampleTimes = false;
                std::shared_ptr<SampleTime> currentSampleTime;
                for (const auto& discreteSampleTime : discreteSampleTimes)
                {
                    if (discreteSampleTime->GetDiscreteSampleTime() == block->GetSampleTime()->GetDiscreteSampleTime())
                    {
                        isAlreadyOnDiscreteSampleTimes = true;
                        currentSampleTime = discreteSampleTime;
                        break;
                    }
                }

                if (!isAlreadyOnDiscreteSampleTimes)
                {
                    discreteSampleTimes.push_back(block->GetSampleTime());
                    blocksForEachDiscreteSampleTime.insert({block->GetSampleTime(), std::vector<std::shared_ptr<ISimulationBlock>>({block})});
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

        // Calculate time hits and associated sample times
        std::vector<std::map<double, std::vector<std::shared_ptr<SampleTime>>>> timeHitsWithSampleTimes;

        // Step 1: Generate uniform time vectors for each discrete sample time
        std::map<double, std::vector<std::shared_ptr<SampleTime>>> timeToSampleTimes;

        for (const auto& sampleTime : discreteSampleTimes)
        {
            double samplePeriod = sampleTime->GetDiscreteSampleTime();
            for (double t = startTime; t < endTime; t += samplePeriod)
            {
                // Associate the sample time with the time hit
                timeToSampleTimes[t].push_back(sampleTime);
            }
        }

        // Step 2: Populate timeHitsWithSampleTimes
        for (const auto& [time, associatedSampleTimes] : timeToSampleTimes)
        {
            timeHitsWithSampleTimes.push_back({{time, associatedSampleTimes}});
        }

        // Step 3: Sort time hits by time
        std::sort(timeHitsWithSampleTimes.begin(), timeHitsWithSampleTimes.end(),
                [](const std::map<double, std::vector<std::shared_ptr<SampleTime>>>& a,
                    const std::map<double, std::vector<std::shared_ptr<SampleTime>>>& b) {
                    return a.begin()->first < b.begin()->first;
                });

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

        for (const auto& timeHit : timeHitsWithSampleTimes)
        {
            for (const auto& [time, sampleTimes] : timeHit)
            {
                for (const auto& sampleTime : sampleTimes)
                {
                    for (auto& block : blocksForEachDiscreteSampleTime[sampleTime])
                    {
                        spdlog::get("default_pysyslink")->debug("Processing block: {} at time {}", block->GetId(), time);
                        block->ComputeOutputsOfBlock(sampleTime);
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
    }
}