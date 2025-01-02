#include "SimulationManager.h"
#include "spdlog/spdlog.h"
#include <thread>
#include <chrono>
#include "BasicOdeSolver.h"
#include "EulerForwardStepSolver.h"
#include <limits>

namespace PySysLinkBase
{
    SimulationManager::SimulationManager(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<SimulationOptions> simulationOptions)
                                        : simulationModel(simulationModel), simulationOptions(simulationOptions)
    {
        std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
        
        this->blocksForEachDiscreteSampleTime = {};
        this->blocksForEachContinuousSampleTimeGroup = {};
        this->blocksWithConstantSampleTime = {};
        
        this->ClasificateBlocks(orderedBlocks, blocksForEachDiscreteSampleTime, blocksWithConstantSampleTime, blocksForEachContinuousSampleTimeGroup);
        spdlog::get("default_pysyslink")->debug("Different discrete sample times: {}", blocksForEachDiscreteSampleTime.size());
        spdlog::get("default_pysyslink")->debug("Blocks with constant sample time: {}", blocksWithConstantSampleTime.size());
        spdlog::get("default_pysyslink")->debug("Different continuous sample times: {}", blocksForEachContinuousSampleTimeGroup.size());

        for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachContinuousSampleTimeGroup.begin(); iter != blocksForEachContinuousSampleTimeGroup.end(); ++iter)
        {
            std::shared_ptr<IOdeStepSolver> odeStepSolver = std::make_shared<EulerForwardStepSolver>();
            std::shared_ptr<BasicOdeSolver> odeSolver = std::make_shared<BasicOdeSolver>(odeStepSolver, this->simulationModel, iter->second, iter->first);
            this->odeSolversForEachContinuousSampleTimeGroup.insert({iter->first, odeSolver});
        }

        this->GetTimeHitsToSampleTimes(simulationOptions, blocksForEachDiscreteSampleTime);
    }


    void SimulationManager::ClasificateBlocks(std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks, 
                                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachDiscreteSampleTime,
                                            std::vector<std::shared_ptr<ISimulationBlock>>& blocksWithConstantSampleTime,
                                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachContinuousSampleTimeGroup)
    {
        for (const auto& block : orderedBlocks)
        {
            spdlog::get("default_pysyslink")->debug("Block {} has sample time {}", block->GetId(), SampleTime::SampleTimeTypeString(block->GetSampleTime()->GetSampleTimeType()));
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
                spdlog::get("default_pysyslink")->debug("Block with continuous sample time: {}", block->GetId());

                bool isAlreadyOnContinuousSampleTimes = false;
                std::shared_ptr<SampleTime> currentSampleTime;
                for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachContinuousSampleTimeGroup.begin(); iter != blocksForEachContinuousSampleTimeGroup.end(); ++iter)
                {
                    if (iter->first->GetContinuousSampleTimeGroup() == block->GetSampleTime()->GetContinuousSampleTimeGroup())
                    {
                        isAlreadyOnContinuousSampleTimes = true;
                        currentSampleTime = iter->first;
                        break;
                    }
                }

                if (!isAlreadyOnContinuousSampleTimes)
                {
                    spdlog::get("default_pysyslink")->debug("Inserting onto dict");

                    blocksForEachContinuousSampleTimeGroup.insert({block->GetSampleTime(), std::vector<std::shared_ptr<ISimulationBlock>>({block})});
                } else {
                    spdlog::get("default_pysyslink")->debug("Seems to be in dict, push back");

                    blocksForEachContinuousSampleTimeGroup[currentSampleTime].push_back(block);
                }
            }
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::constant)
            {
                blocksWithConstantSampleTime.push_back(block);
            }
        }
    }

    void SimulationManager::GetTimeHitsToSampleTimes(std::shared_ptr<SimulationOptions> simulationOptions, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime)
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

        this->timeHitsToSampleTimes = timeHitsToSampleTimes;
        this->timeHits = {};

        for (const auto& [time, sampleTimes] : timeHitsToSampleTimes)
        {
            this->timeHits.push_back(time);
        }
    }

    void SimulationManager::RunSimulation()
    {        
        auto simulationStartTime = std::chrono::system_clock::now();
        double currentTime = simulationOptions->startTime;
        spdlog::get("default_pysyslink")->debug("Simulation start");

        int nextDiscreteTimeHitToProcessIndex = 0;

        for (auto& block : blocksWithConstantSampleTime)
        {
            this->ProcessBlock(simulationModel, block, block->GetSampleTime(), currentTime);
        }

        for (std::map<std::shared_ptr<SampleTime>, std::shared_ptr<BasicOdeSolver>>::iterator iter = this->odeSolversForEachContinuousSampleTimeGroup.begin(); iter != this->odeSolversForEachContinuousSampleTimeGroup.end(); ++iter)
        {
            spdlog::get("default_pysyslink")->debug("First simulation step with continuous blocks of group {}", iter->first->GetContinuousSampleTimeGroup());
            iter->second->DoStep(currentTime, 1e-2);
        }
        
        spdlog::get("default_pysyslink")->debug("Main simulation loop start");
        while (currentTime <= simulationOptions->stopTime)
        {
            std::tuple<double, int, std::vector<std::shared_ptr<SampleTime>>> timeIndexAndSampleTimes = this->GetNearestTimeHit(nextDiscreteTimeHitToProcessIndex);
            double nearestTimeHit = std::get<0>(timeIndexAndSampleTimes);
            nextDiscreteTimeHitToProcessIndex = std::get<1>(timeIndexAndSampleTimes);
            std::vector<std::shared_ptr<SampleTime>> sampleTimesToProcess = std::get<2>(timeIndexAndSampleTimes);

            currentTime = nearestTimeHit;
            auto targetTime = simulationStartTime + std::chrono::duration<double>(currentTime/simulationOptions->naturalTimeSpeedMultiplier);
            if (simulationOptions->runInNaturalTime)
            {
                std::this_thread::sleep_until(targetTime);
                auto actualTime = std::chrono::system_clock::now();
                auto elapsedRealTime = std::chrono::duration<double>(actualTime - simulationStartTime).count();
                spdlog::get("default_pysyslink")->debug("Simulated Time: {}, Real Time Elapsed: {} seconds", currentTime, elapsedRealTime);            
            }

            for (const auto& sampleTime : sampleTimesToProcess)
            {
                if (sampleTime->GetSampleTimeType() == SampleTimeType::discrete)
                {
                    for (auto& block : blocksForEachDiscreteSampleTime[sampleTime])
                    {
                        this->ProcessBlock(simulationModel, block, sampleTime, currentTime);
                    }
                }
                else if (sampleTime->GetSampleTimeType() == SampleTimeType::continuous)
                {
                    auto odeSolver = this->odeSolversForEachContinuousSampleTimeGroup[sampleTime];
                    odeSolver->DoStep(currentTime, odeSolver->GetNextSuggestedTimeStep());
                }
            }
        }
        spdlog::get("default_pysyslink")->debug("Simulation end");
    }

    std::tuple<double, int, std::vector<std::shared_ptr<SampleTime>>> SimulationManager::GetNearestTimeHit(int nextDiscreteTimeHitToProcessIndex)
    {
        double nearestTimeHit = std::numeric_limits<double>::quiet_NaN();
        std::vector<std::shared_ptr<SampleTime>> sampleTimesToProcess = {};

        spdlog::get("default_pysyslink")->debug("Time hits size: {}", this->timeHits.size());
        if (this->timeHits.size() != 0)
        {
            nearestTimeHit = this->timeHits[nextDiscreteTimeHitToProcessIndex];
            sampleTimesToProcess = this->timeHitsToSampleTimes[this->timeHits[nextDiscreteTimeHitToProcessIndex]];
        }

        for (std::map<std::shared_ptr<SampleTime>, std::shared_ptr<BasicOdeSolver>>::iterator iter = this->odeSolversForEachContinuousSampleTimeGroup.begin(); iter != this->odeSolversForEachContinuousSampleTimeGroup.end(); ++iter)
        {
            spdlog::get("default_pysyslink")->debug("Looking on continuous sample time...");

            double nextTimeHit_i = iter->second->GetNextTimeHit();
            
            if (std::isnan(nearestTimeHit))
            {
                nearestTimeHit = nextTimeHit_i;
                sampleTimesToProcess = {iter->first};
            }
            else if (nextTimeHit_i < nearestTimeHit)
            {
                nearestTimeHit = nextTimeHit_i;
                sampleTimesToProcess = {iter->first};
            }
            else if (nextTimeHit_i == nearestTimeHit)
            {
                sampleTimesToProcess.push_back(iter->first);
            }
        }

        if (this->timeHits.size() != 0)
        {
            if (nearestTimeHit == this->timeHits[nextDiscreteTimeHitToProcessIndex])
            {
                nextDiscreteTimeHitToProcessIndex += 1;
                if (nextDiscreteTimeHitToProcessIndex > this->timeHits.size())
                {
                    spdlog::get("default_pysyslink")->debug("Too much index to look for");
                    return {0.0, -1, sampleTimesToProcess};
                }
            }
        }

        return {nearestTimeHit, nextDiscreteTimeHitToProcessIndex, sampleTimesToProcess};
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