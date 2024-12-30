#include "SimulationManager.h"
#include "spdlog/spdlog.h"
#include <thread>
#include <chrono>

namespace PySysLinkBase
{
    void SimulationManager::ClasificateBlocks(std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks, 
                                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachDiscreteSampleTime,
                                            std::vector<std::shared_ptr<ISimulationBlock>>& blocksWithConstantSampleTime,
                                            std::map<int, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachContinuousSampleTimeGroup)
    {
        for (const auto& block : orderedBlocks)
        {
            if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::discrete)
            {
                bool isAlreadyOnDiscreteSampleTimes = false;
                std::shared_ptr<SampleTime> currentSampleTime;
                for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachDiscreteSampleTime.begin(); iter != blocksForEachDiscreteSampleTime.end(); ++iter)
                {
                    if (iter->first->GetDiscreteSampleTime() == block->GetSampleTime()->GetDiscreteSampleTime())
                    {
                        isAlreadyOnDiscreteSampleTimes = true;
                        currentSampleTime = iter->first;
                        break;
                    }
                }

                if (!isAlreadyOnDiscreteSampleTimes)
                {
                    blocksForEachDiscreteSampleTime.insert({block->GetSampleTime(), std::vector<std::shared_ptr<ISimulationBlock>>({block})});
                }
                else
                {
                    blocksForEachDiscreteSampleTime[currentSampleTime].push_back(block);
                }
            }
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::continuous)
            {
                int continuousSampleTimeGroup = block->GetSampleTime()->GetContinuousSampleTimeGroup();
                if (blocksForEachContinuousSampleTimeGroup.find(continuousSampleTimeGroup) == blocksForEachContinuousSampleTimeGroup.end()) {
                    blocksForEachContinuousSampleTimeGroup.insert({continuousSampleTimeGroup, std::vector<std::shared_ptr<ISimulationBlock>>({block})});
                } else {
                    blocksForEachContinuousSampleTimeGroup[continuousSampleTimeGroup].push_back(block);
                }
            }
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::constant)
            {
                blocksWithConstantSampleTime.push_back(block);
            }
        }
    }

    std::map<double, std::vector<std::shared_ptr<SampleTime>>> SimulationManager::GetTimeHitsToSampleTimes(std::shared_ptr<SimulationOptions> simulationOptions, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime)
    {
        std::map<double, std::vector<std::shared_ptr<SampleTime>>> timeHitsToSampleTimes;

        for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachDiscreteSampleTime.begin(); iter != blocksForEachDiscreteSampleTime.end(); ++iter)
        {
            double samplePeriod = iter->first->GetDiscreteSampleTime();
            for (double t = simulationOptions->startTime; t <= simulationOptions->stopTime; t += samplePeriod)
            {
                if (timeHitsToSampleTimes.find(t) == timeHitsToSampleTimes.end()) 
                {
                    timeHitsToSampleTimes.insert({t, std::vector<std::shared_ptr<SampleTime>>({iter->first})});
                } else {
                    timeHitsToSampleTimes[t].push_back(iter->first);
                }
            }
        }

        return timeHitsToSampleTimes;
    }

    void SimulationManager::RunSimulation(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<SimulationOptions> simulationOptions)
    {
        std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
        
        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime = {};
        std::map<int, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachContinuousSampleTimeGroup = {};
        std::vector<std::shared_ptr<ISimulationBlock>> blocksWithConstantSampleTime = {};
        
        this->ClasificateBlocks(orderedBlocks, blocksForEachDiscreteSampleTime, blocksWithConstantSampleTime, blocksForEachContinuousSampleTimeGroup);
        spdlog::get("default_pysyslink")->debug("Different discrete sample times: {}", blocksForEachDiscreteSampleTime.size());
        spdlog::get("default_pysyslink")->debug("Blocks with constant sample time: {}", blocksWithConstantSampleTime.size());

        
        std::map<double, std::vector<std::shared_ptr<SampleTime>>> timeHitsToSampleTimes = this->GetTimeHitsToSampleTimes(simulationOptions, blocksForEachDiscreteSampleTime);


        auto simulationStartTime = std::chrono::system_clock::now();
        double currentTime = simulationOptions->startTime;
        spdlog::get("default_pysyslink")->debug("Simulation start");

        for (auto& block : blocksWithConstantSampleTime)
        {
            this->ProcessBlock(simulationModel, block, block->GetSampleTime(), currentTime);
        }

        while (currentTime <= simulationOptions->stopTime)
        {
            
        }
        for (const auto& [time, sampleTimes] : timeHitsToSampleTimes)
        {
            currentTime = time;
            auto targetTime = simulationStartTime + std::chrono::duration<double>(time/simulationOptions->naturalTimeSpeedMultiplier);
            if (simulationOptions->runInNaturalTime)
            {
                std::this_thread::sleep_until(targetTime);
                auto actualTime = std::chrono::system_clock::now();
                auto elapsedRealTime = std::chrono::duration<double>(actualTime - simulationStartTime).count();
                spdlog::get("default_pysyslink")->debug("Simulated Time: {}, Real Time Elapsed: {} seconds", currentTime, elapsedRealTime);            
            }

            for (const auto& sampleTime : sampleTimes)
            {
                for (auto& block : blocksForEachDiscreteSampleTime[sampleTime])
                {
                    this->ProcessBlock(simulationModel, block, sampleTime, currentTime);
                }
            }
        }
    }

    void SimulationManager::ProcessBlock(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        spdlog::get("default_pysyslink")->debug("Processing block: {} at time {}", block->GetId(), currentTime);
        block->ComputeOutputsOfBlock(sampleTime, currentTime);
        for (int i = 0; i < block->GetOutputPorts().size(); i++)
        {
            for (auto& connectedPort : simulationModel->GetConnectedPorts(block, i))
            {
                block->GetOutputPorts()[i]->TryCopyValueToPort(*connectedPort);
            }
        }
    }
}