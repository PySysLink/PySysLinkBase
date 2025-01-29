#include "SimulationManager.h"
#include "spdlog/spdlog.h"
#include <thread>
#include <chrono>
#include "ContinuousAndOde/BasicOdeSolver.h"
#include "PortsAndSignalValues/SignalValue.h"
#include "ContinuousAndOde/EulerForwardStepSolver.h"
#include "ContinuousAndOde/OdeintStepSolver.h"
#include "ContinuousAndOde/SolverFactory.h"
#include <limits>
#include <iostream>

namespace PySysLinkBase
{
    SimulationManager::SimulationManager(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<SimulationOptions> simulationOptions)
                                        : simulationModel(simulationModel), simulationOptions(simulationOptions)
    {
        std::vector<std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>>> blockChains = simulationModel->GetDirectBlockChains();
        this->orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(blockChains);
        
        this->blocksForEachDiscreteSampleTime = {};
        this->blocksForEachContinuousSampleTimeGroup = {};
        this->blocksWithConstantSampleTime = {};
        
        this->ClasificateBlocks(orderedBlocks, blocksForEachDiscreteSampleTime, blocksWithConstantSampleTime, blocksForEachContinuousSampleTimeGroup);
        spdlog::get("default_pysyslink")->debug("Different discrete sample times: {}", blocksForEachDiscreteSampleTime.size());
        spdlog::get("default_pysyslink")->debug("Blocks with constant sample time: {}", blocksWithConstantSampleTime.size());
        spdlog::get("default_pysyslink")->debug("Different continuous sample times: {}", blocksForEachContinuousSampleTimeGroup.size());

        for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachContinuousSampleTimeGroup.begin(); iter != blocksForEachContinuousSampleTimeGroup.end(); ++iter)
        {
            std::shared_ptr<IOdeStepSolver> odeStepSolver;
            std::string selectedKey = "default";            
            if (this->simulationOptions->solversConfiguration.find(std::to_string(iter->first->GetContinuousSampleTimeGroup())) == this->simulationOptions->solversConfiguration.end())
            {
                if (this->simulationOptions->solversConfiguration.find("default") == this->simulationOptions->solversConfiguration.end())
                {
                    throw std::invalid_argument("Solver for continuous sample time " + std::to_string(iter->first->GetContinuousSampleTimeGroup()) + " not found in configuration, and no default solver was provided.");
                }
                else
                {
                    spdlog::get("default_pysyslink")->info("Solver for continuous sample time {} not found in configuration, using default solver.", iter->first->GetContinuousSampleTimeGroup());
                    selectedKey = "default";
                }
            }
            else
            {
                selectedKey = std::to_string(iter->first->GetContinuousSampleTimeGroup());
            }

            double firstTimeStep = 1e-6;
            try
            {
                firstTimeStep = ConfigurationValueManager::TryGetConfigurationValue<double>("FirstTimeStep", this->simulationOptions->solversConfiguration[selectedKey]);
            }
            catch (std::out_of_range const& ex)
            {
                spdlog::get("default_pysyslink")->debug("First time step not found in configuration, using default value.");
            }

            odeStepSolver = SolverFactory::CreateOdeStepSolver(this->simulationOptions->solversConfiguration[selectedKey]);

            std::shared_ptr<BasicOdeSolver> odeSolver = std::make_shared<BasicOdeSolver>(odeStepSolver, this->simulationModel, iter->second, iter->first, this->simulationOptions, firstTimeStep);
            this->odeSolversForEachContinuousSampleTimeGroup.insert({iter->first, odeSolver});
        }

        this->GetTimeHitsToSampleTimes(simulationOptions, blocksForEachDiscreteSampleTime);

        this->simulationOutput = std::make_shared<SimulationOutput>();
        this->simulationModel->blockEventsHandler->RegisterValueUpdateBlockEventCallback(std::bind(&SimulationManager::ValueUpdateBlockEventCallback, this, std::placeholders::_1));

        for (const auto& blockIdInputOrOutputAndIndexeToLog : simulationOptions->blockIdsInputOrOutputAndIndexesToLog)
        {
            std::string blockId = std::get<0>(blockIdInputOrOutputAndIndexeToLog);
            std::string inputOrOutput = std::get<1>(blockIdInputOrOutputAndIndexeToLog);
            int outputIndex = std::get<2>(blockIdInputOrOutputAndIndexeToLog);
            std::shared_ptr<ISimulationBlock> block = ISimulationBlock::FindBlockById(blockId, this->simulationModel->simulationBlocks);
            if (inputOrOutput == "input")
            {
                block->RegisterReadInputsCallbacks(std::bind(&SimulationManager::LogSignalInputReadCallback, this, std::placeholders::_1, std::placeholders::_2, outputIndex, std::placeholders::_3, std::placeholders::_4));
            }
            else if (inputOrOutput == "output")
            {
                block->RegisterCalculateOutputCallbacks(std::bind(&SimulationManager::LogSignalOutputUpdateCallback, this, std::placeholders::_1, std::placeholders::_2, outputIndex, std::placeholders::_3, std::placeholders::_4));
            }
            else
            {
                spdlog::get("default_pysyslink")->error("Invalid input or output type in signal log configuration: {}", inputOrOutput);
            }        
        }
    }

    void SimulationManager::LogSignalOutputUpdateCallback(const std::string blockId, const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts, int outputPortIndex, std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime)
    {
        std::string signalId = blockId + "/output/" + std::to_string(outputPortIndex);
        if (this->simulationOutput->signals.find("LoggedSignals") == this->simulationOutput->signals.end())
        {
            this->simulationOutput->signals.insert({"LoggedSignals", std::map<std::string, std::shared_ptr<UnknownTypeSignal>>()});
        }
        
        try
        {
            double valueDouble = outputPorts[outputPortIndex]->GetValue()->TryCastToTyped<double>()->GetPayload();

            if (this->simulationOutput->signals["LoggedSignals"].find(signalId) == this->simulationOutput->signals["LoggedSignals"].end())
            {
                this->simulationOutput->signals["LoggedSignals"].insert({signalId, std::make_shared<Signal<double>>()});
            }
            this->simulationOutput->signals["LoggedSignals"][signalId]->TryInsertValue<double>(this->currentTime, valueDouble);
        }
        catch(const std::bad_variant_access& e)
        {
            std::complex<double> valueComplex = outputPorts[outputPortIndex]->GetValue()->TryCastToTyped<std::complex<double>>()->GetPayload();

            if (this->simulationOutput->signals["LoggedSignals"].find(signalId) == this->simulationOutput->signals["LoggedSignals"].end())
            {
                this->simulationOutput->signals["LoggedSignals"].insert({signalId, std::make_shared<Signal<std::complex<double>>>()});
            }
            this->simulationOutput->signals["LoggedSignals"][signalId]->TryInsertValue<std::complex<double>>(this->currentTime, valueComplex);
        }
    }
    void SimulationManager::LogSignalInputReadCallback(const std::string blockId, const std::vector<std::shared_ptr<PySysLinkBase::InputPort>> inputPorts, int inputPortIndex, std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime)
    {
        std::string signalId = blockId + "/input/" + std::to_string(inputPortIndex);
        if (this->simulationOutput->signals.find("LoggedSignals") == this->simulationOutput->signals.end())
        {
            this->simulationOutput->signals.insert({"LoggedSignals", std::map<std::string, std::shared_ptr<UnknownTypeSignal>>()});
        }
        
        try
        {
            double valueDouble = inputPorts[inputPortIndex]->GetValue()->TryCastToTyped<double>()->GetPayload();

            if (this->simulationOutput->signals["LoggedSignals"].find(signalId) == this->simulationOutput->signals["LoggedSignals"].end())
            {
                this->simulationOutput->signals["LoggedSignals"].insert({signalId, std::make_shared<Signal<double>>()});
            }
            this->simulationOutput->signals["LoggedSignals"][signalId]->TryInsertValue<double>(this->currentTime, valueDouble);
        }
        catch(const std::bad_variant_access& e)
        {
            std::complex<double> valueComplex = inputPorts[inputPortIndex]->GetValue()->TryCastToTyped<std::complex<double>>()->GetPayload();

            if (this->simulationOutput->signals["LoggedSignals"].find(signalId) == this->simulationOutput->signals["LoggedSignals"].end())
            {
                this->simulationOutput->signals["LoggedSignals"].insert({signalId, std::make_shared<Signal<std::complex<double>>>()});
            }
            this->simulationOutput->signals["LoggedSignals"][signalId]->TryInsertValue<std::complex<double>>(this->currentTime, valueComplex);
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
        auto insertBlockInDiscreteSampleTime = [](const std::shared_ptr<ISimulationBlock> block, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachDiscreteSampleTime, std::shared_ptr<SampleTime> multirateSampleTime=nullptr) -> void {
            bool isAlreadyOnDiscreteSampleTimes = false;
            std::shared_ptr<SampleTime> currentSampleTime;

            std::shared_ptr<SampleTime> keySampleTime;
            if (multirateSampleTime == nullptr)
            {
                keySampleTime = block->GetSampleTime();
            }
            else
            {
                keySampleTime = multirateSampleTime;
            }

            for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachDiscreteSampleTime.begin(); iter != blocksForEachDiscreteSampleTime.end(); ++iter)
            {
                if (iter->first->GetDiscreteSampleTime() == keySampleTime->GetDiscreteSampleTime())
                {
                    isAlreadyOnDiscreteSampleTimes = true;
                    currentSampleTime = iter->first;
                    break;
                }
            }

            if (!isAlreadyOnDiscreteSampleTimes)
            {
                blocksForEachDiscreteSampleTime.insert({keySampleTime, std::vector<std::shared_ptr<ISimulationBlock>>({block})});
            }
            else
            {
                blocksForEachDiscreteSampleTime[currentSampleTime].push_back(block);
            }
        };

        auto insertBlockInContinuousSampleTime = [](const std::shared_ptr<ISimulationBlock> block, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachContinuousSampleTimeGroup, std::shared_ptr<SampleTime> multirateSampleTime=nullptr) -> void {
            spdlog::get("default_pysyslink")->debug("Block with continuous sample time: {}", block->GetId());

            bool isAlreadyOnContinuousSampleTimes = false;
            std::shared_ptr<SampleTime> currentSampleTime;

            std::shared_ptr<SampleTime> keySampleTime;
            if (multirateSampleTime == nullptr)
            {
                keySampleTime = block->GetSampleTime();
            }
            else
            {
                keySampleTime = multirateSampleTime;
            }

            for (std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>::iterator iter = blocksForEachContinuousSampleTimeGroup.begin(); iter != blocksForEachContinuousSampleTimeGroup.end(); ++iter)
            {
                if (iter->first->GetContinuousSampleTimeGroup() == keySampleTime->GetContinuousSampleTimeGroup())
                {
                    isAlreadyOnContinuousSampleTimes = true;
                    currentSampleTime = iter->first;
                    break;
                }
            }

            if (!isAlreadyOnContinuousSampleTimes)
            {
                spdlog::get("default_pysyslink")->debug("Inserting onto dict");

                blocksForEachContinuousSampleTimeGroup.insert({keySampleTime, std::vector<std::shared_ptr<ISimulationBlock>>({block})});
            } else {
                spdlog::get("default_pysyslink")->debug("Seems to be in dict, push back");

                blocksForEachContinuousSampleTimeGroup[currentSampleTime].push_back(block);
            }
        };

        for (const auto& block : orderedBlocks)
        {
            spdlog::get("default_pysyslink")->debug("Block {} has sample time {}", block->GetId(), SampleTime::SampleTimeTypeString(block->GetSampleTime()->GetSampleTimeType()));
            if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::discrete)
            {
                insertBlockInDiscreteSampleTime(block, blocksForEachDiscreteSampleTime);
            }
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::continuous)
            {
                insertBlockInContinuousSampleTime(block, blocksForEachContinuousSampleTimeGroup);
            }
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::constant)
            {
                blocksWithConstantSampleTime.push_back(block);
            }
            else if (block->GetSampleTime()->GetSampleTimeType() == SampleTimeType::multirate)
            {
                for (const auto& sampleTime : block->GetSampleTime()->GetMultirateSampleTimes())
                {
                    if (sampleTime->GetSampleTimeType() == SampleTimeType::discrete)
                    {
                        insertBlockInDiscreteSampleTime(block, blocksForEachDiscreteSampleTime, sampleTime);
                    }
                    else if (sampleTime->GetSampleTimeType() == SampleTimeType::continuous)
                    {
                        insertBlockInContinuousSampleTime(block, blocksForEachContinuousSampleTimeGroup, sampleTime);
                    }
                    else if (sampleTime->GetSampleTimeType() == SampleTimeType::constant)
                    {
                        blocksWithConstantSampleTime.push_back(block);
                    }
                    else
                    {
                        throw std::invalid_argument("Sample time of type " + SampleTime::SampleTimeTypeString(sampleTime->GetSampleTimeType()) + " should not be in simulation manager.");
                    }
                }
            }
            else
            {
                throw std::invalid_argument("Sample time of type " + SampleTime::SampleTimeTypeString(block->GetSampleTime()->GetSampleTimeType()) + " should not be in simulation manager.");
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
            iter->second->DoStep(currentTime, iter->second->firstTimeStep);
            iter->second->ComputeMajorOutputs(currentTime);
        }
        
        spdlog::get("default_pysyslink")->debug("Main simulation loop start");
        while (currentTime <= simulationOptions->stopTime)
        {
            std::tuple<double, int, std::vector<std::shared_ptr<SampleTime>>> timeIndexAndSampleTimes = this->GetNearestTimeHit(nextDiscreteTimeHitToProcessIndex);
            double nearestTimeHit = std::get<0>(timeIndexAndSampleTimes);
            nextDiscreteTimeHitToProcessIndex = std::get<1>(timeIndexAndSampleTimes);
            std::vector<std::shared_ptr<SampleTime>> sampleTimesToProcess = std::get<2>(timeIndexAndSampleTimes);
            
            if (nextDiscreteTimeHitToProcessIndex == -1)
            {
                break;
            }

            currentTime = nearestTimeHit;
            spdlog::get("default_pysyslink")->debug("Current time: {}", currentTime);

            if (simulationOptions->runInNaturalTime)
            {
                auto targetTime = simulationStartTime + std::chrono::duration<double>(currentTime/simulationOptions->naturalTimeSpeedMultiplier);

                std::this_thread::sleep_until(targetTime);
                auto actualTime = std::chrono::system_clock::now();
                auto elapsedRealTime = std::chrono::duration<double>(actualTime - simulationStartTime).count();
                spdlog::get("default_pysyslink")->debug("Simulated Time: {}, Real Time Elapsed: {} seconds", currentTime, elapsedRealTime);            
            }

            if (sampleTimesToProcess.size() < 2)
            {
                for (const auto& sampleTime : sampleTimesToProcess)
                {
                    spdlog::get("default_pysyslink")->debug("Solving sample time of type: {}", SampleTime::SampleTimeTypeString(sampleTime->GetSampleTimeType()));            
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
                        odeSolver->ComputeMajorOutputs(currentTime);
                    }
                }
            }
            else
            {
                spdlog::get("default_pysyslink")->debug("Solving many sample times"); 

                for (const auto& sampleTime : sampleTimesToProcess)
                {
                    if (sampleTime->GetSampleTimeType() == SampleTimeType::continuous)
                    {
                        auto odeSolver = this->odeSolversForEachContinuousSampleTimeGroup[sampleTime];
                        odeSolver->UpdateStatesToNextTimeHits(); // So that the output of each block can be correctly calculated
                    }
                }           
                
                this->ProcessBlocksInSampleTimes(sampleTimesToProcess, true);

                for (const auto& sampleTime : sampleTimesToProcess)
                {
                    if (sampleTime->GetSampleTimeType() == SampleTimeType::continuous)
                    {
                        auto odeSolver = this->odeSolversForEachContinuousSampleTimeGroup[sampleTime];
                        odeSolver->DoStep(currentTime, odeSolver->GetNextSuggestedTimeStep());
                    }
                }

                this->ProcessBlocksInSampleTimes(sampleTimesToProcess, false);                
            }
        }
        spdlog::get("default_pysyslink")->debug("Simulation end");

        return this->simulationOutput;
    }

    void SimulationManager::ProcessBlocksInSampleTimes(const std::vector<std::shared_ptr<SampleTime>> sampleTimes, bool isMinorStep)
    {
        for (const auto& block : this->orderedBlocks)
        {
            auto sampleTime = block->GetSampleTime();
            
            bool processBlock = false;
            if (sampleTime->GetSampleTimeType() == SampleTimeType::discrete)
            {
                processBlock = this->IsBlockInSampleTimes(block, sampleTimes, this->blocksForEachDiscreteSampleTime);
            }
            else if (sampleTime->GetSampleTimeType() == SampleTimeType::continuous)
            {
                processBlock = this->IsBlockInSampleTimes(block, sampleTimes, this->blocksForEachContinuousSampleTimeGroup);
            }
            else if (sampleTime->GetSampleTimeType() == SampleTimeType::multirate)
            {
                bool processBlock1 = this->IsBlockInSampleTimes(block, sampleTimes, this->blocksForEachDiscreteSampleTime);
                bool processBlock2 = this->IsBlockInSampleTimes(block, sampleTimes, this->blocksForEachContinuousSampleTimeGroup);
                processBlock = processBlock1 || processBlock2;
            }
            // Only process if the sample time is in the current list to process
            if (processBlock)
            {
                spdlog::get("default_pysyslink")->debug("Block to process on multiple time hit: {}", block->GetId()); 

                this->ProcessBlock(simulationModel, block, sampleTime, currentTime, isMinorStep);
            }
        }
    }


    bool SimulationManager::IsBlockInSampleTimes(const std::shared_ptr<ISimulationBlock>& block, const std::vector<std::shared_ptr<SampleTime>>& sampleTimes, 
                                            const std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blockMap)
    {
        for (const auto& sampleTime : sampleTimes)
        {
            auto it = blockMap.find(sampleTime);
            if (it != blockMap.end())
            {
                // Use std::find to check if the block exists in the vector
                if (std::find(it->second.begin(), it->second.end(), block) != it->second.end())
                {
                    return true;  // Block found in this sample time
                }
            }
        }
        return false;  // Block not found in any sample time
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
                spdlog::get("default_pysyslink")->debug("New continuous sample time hit at the same moment!");
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

    void SimulationManager::ProcessBlock(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime, bool isMinorStep)
    {
        spdlog::get("default_pysyslink")->debug("Processing block: {} at time {}", block->GetId(), currentTime);
        block->ComputeOutputsOfBlock(sampleTime, currentTime, isMinorStep);
        for (int i = 0; i < block->GetOutputPorts().size(); i++)
        {
            for (auto& connectedPort : simulationModel->GetConnectedPorts(block, i))
            {
                block->GetOutputPorts()[i]->TryCopyValueToPort(*connectedPort);
            }
        }
    }
}