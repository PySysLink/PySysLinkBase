#include "SimulationManager.h"
#include "spdlog/spdlog.h"
#include <thread>
#include <chrono>
#include "BasicOdeSolver.h"
#include "PortsAndSignalValues/SignalValue.h"
#include "EulerForwardStepSolver.h"
#include <limits>
#include <iostream>

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

        this->simulationOutput = std::make_shared<SimulationOutput>();
        this->simulationModel->blockEventsHandler->RegisterValueUpdateBlockEventCallback(std::bind(&SimulationManager::ValueUpdateBlockEventCallback, this, std::placeholders::_1));

        for (const auto& blockIdAndOutputIndexToLog : simulationOptions->blockIdsAndOutputIndexesToLog)
        {
            std::string blockId = blockIdAndOutputIndexToLog.first;
            int outputIndex = blockIdAndOutputIndexToLog.second;
            std::shared_ptr<ISimulationBlock> block = ISimulationBlock::FindBlockById(blockId, this->simulationModel->simulationBlocks);
            std::shared_ptr<OutputPort> outputPort = block->GetOutputPorts()[outputIndex];

            outputPort->RegisterCopyCallback(std::bind(&SimulationManager::PortToLogCopyCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

            this->loggedPortToCorrespondentBlockIdAndOutputPortIndex.insert({outputPort.get(), std::make_pair(blockId, outputIndex)});
        }

    }

    void SimulationManager::PortToLogCopyCallback(const Port &thisPort, const Port &otherPort, std::shared_ptr<UnknownTypeSignalValue> value)
    {
        if (this->portToLogInToAvoidRepetition.find(&thisPort) == this->portToLogInToAvoidRepetition.end())
        {
            this->portToLogInToAvoidRepetition.insert({&thisPort, &otherPort});
        }

        if (&otherPort == this->portToLogInToAvoidRepetition[&thisPort])
        {
            std::pair<std::string, int> blockIdAndOutputIndex = this->loggedPortToCorrespondentBlockIdAndOutputPortIndex[&thisPort];
            std::string signalId = blockIdAndOutputIndex.first + "/" + std::to_string(blockIdAndOutputIndex.second);
            if (this->simulationOutput->signals.find("LoggedSignals") == this->simulationOutput->signals.end())
            {
                this->simulationOutput->signals.insert({"LoggedSignals", std::map<std::string, std::shared_ptr<UnknownTypeSignal>>()});
            }
            
            try
            {
                double valueDouble = value->TryCastToTyped<double>()->GetPayload();

                if (this->simulationOutput->signals["LoggedSignals"].find(signalId) == this->simulationOutput->signals["LoggedSignals"].end())
                {
                    this->simulationOutput->signals["LoggedSignals"].insert({signalId, std::make_shared<Signal<double>>()});
                }
                this->simulationOutput->signals["LoggedSignals"][signalId]->TryInsertValue<double>(this->currentTime, valueDouble);
            }
            catch(const std::bad_variant_access& e)
            {
                std::complex<double> valueComplex = value->TryCastToTyped<std::complex<double>>()->GetPayload();

                if (this->simulationOutput->signals["LoggedSignals"].find(signalId) == this->simulationOutput->signals["LoggedSignals"].end())
                {
                    this->simulationOutput->signals["LoggedSignals"].insert({signalId, std::make_shared<Signal<std::complex<double>>>()});
                }
                this->simulationOutput->signals["LoggedSignals"][signalId]->TryInsertValue<std::complex<double>>(this->currentTime, valueComplex);
            }
        }
    }

    void SimulationManager::ValueUpdateBlockEventCallback(const std::shared_ptr<ValueUpdateBlockEvent> blockEvent)
    {
        std::string eventValueId = blockEvent->valueId;
        int firstFinding = eventValueId.find("/");
        int secondFinding = eventValueId.find("/", firstFinding + 1);
        std::string valueEventType = eventValueId.substr(firstFinding + 1, secondFinding - firstFinding - 1);
        std::string displayId = eventValueId.substr(secondFinding + 1, eventValueId.size() - secondFinding - 1);
        spdlog::get("default_pysyslink")->debug("Value update event type: {}", valueEventType);
        spdlog::get("default_pysyslink")->debug("Display id: {}", displayId);

        if (valueEventType == "DisplayValue")
        {
            if (this->simulationOutput->signals.find("Displays") == this->simulationOutput->signals.end())
            {
                this->simulationOutput->signals.insert({"Displays", std::map<std::string, std::shared_ptr<UnknownTypeSignal>>()});
            }
            
            try
            {
                double value = std::get<double>(blockEvent->value);
                if (this->simulationOutput->signals["Displays"].find(displayId) == this->simulationOutput->signals["Displays"].end())
                {
                    this->simulationOutput->signals["Displays"].insert({displayId, std::make_shared<Signal<double>>()});
                }
                this->simulationOutput->signals["Displays"][displayId]->TryInsertValue<double>(blockEvent->simulationTime, value);
            }
            catch(const std::bad_variant_access& e)
            {
                std::complex<double> value = std::get<std::complex<double>>(blockEvent->value);
                if (this->simulationOutput->signals["Displays"].find(displayId) == this->simulationOutput->signals["Displays"].end())
                {
                    this->simulationOutput->signals["Displays"].insert({displayId, std::make_shared<Signal<std::complex<double>>>()});
                }
                this->simulationOutput->signals["Displays"][displayId]->TryInsertValue<std::complex<double>>(blockEvent->simulationTime, std::get<std::complex<double>>(blockEvent->value));
            }
        }
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

    std::shared_ptr<SimulationOutput> SimulationManager::RunSimulation()
    {        
        auto simulationStartTime = std::chrono::system_clock::now();
        this->currentTime = simulationOptions->startTime;
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

        return this->simulationOutput;
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