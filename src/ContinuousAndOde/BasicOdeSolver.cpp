#include "BasicOdeSolver.h"
#include <limits>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace PySysLinkBase
{
    BasicOdeSolver::BasicOdeSolver(std::shared_ptr<IOdeStepSolver> odeStepSolver, std::shared_ptr<SimulationModel> simulationModel, 
                                    std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::shared_ptr<SampleTime> sampleTime, 
                                    std::shared_ptr<SimulationOptions> simulationOptions,
                                    double firstTimeStep, bool activateEvents, double eventTolerance) 
                                    : odeStepSolver(odeStepSolver), simulationModel(simulationModel), simulationBlocks(simulationBlocks), sampleTime(sampleTime), firstTimeStep(firstTimeStep),
                                    activateEvents(activateEvents), eventTolerance(eventTolerance)
    {
        this->continuousStatesInEachBlock = {};
        this->totalStates = 0;
        this->nextUnknownTimeHit = std::numeric_limits<double>::quiet_NaN();
        this->nextSuggestedTimeStep = std::numeric_limits<double>::quiet_NaN();

        for (auto& block : this->simulationBlocks)
        {
            std::shared_ptr<ISimulationBlockWithContinuousStates> blockWithContinuousStates = std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block);
            if (std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block))
            {
                this->continuousStatesInEachBlock.push_back(blockWithContinuousStates->GetContinuousStates().size());
                this->totalStates += blockWithContinuousStates->GetContinuousStates().size();
            }
            else
            {
                this->continuousStatesInEachBlock.push_back(0);
            }

            std::vector<double> knownEvents_i = block->GetKnownEvents(block->GetSampleTime(), simulationOptions->startTime, simulationOptions->stopTime);
            for (const auto& event : knownEvents_i)
            {
                this->knownTimeHits.push_back(event);
            }
        }

        std::sort(std::begin(this->knownTimeHits), std::end(this->knownTimeHits));

        this->nextTimeHitStates = {};
    }

    void BasicOdeSolver::ComputeMinorOutputs(std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        for (auto& block : this->simulationBlocks)
        {
            this->ComputeBlockOutputs(block, sampleTime, currentTime, true);
        }
    }

    void BasicOdeSolver::ComputeMajorOutputs(double currentTime)
    {
        for (auto& block : this->simulationBlocks)
        {
            this->ComputeBlockOutputs(block, this->sampleTime, currentTime, false);
        }
    }

    std::vector<double> BasicOdeSolver::GetStates()
    {
        std::vector<double> states(this->totalStates, 0.0);

        int i = 0;
        int currentIndex = 0;
        for (auto& block : this->simulationBlocks)
        {
            std::shared_ptr<ISimulationBlockWithContinuousStates> blockWithContinuousStates = std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block);
            if (blockWithContinuousStates)
            {
                std::vector<double> states_i = blockWithContinuousStates->GetContinuousStates();
                for (int j = 0; j < states_i.size(); j++)
                {
                    states[currentIndex] = states_i[j];
                    currentIndex++;
                }
            }

            i += 1;
        }
        
        return states;
    }

    std::vector<double> BasicOdeSolver::GetDerivatives(std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        std::vector<double> derivatives(this->totalStates, 0.0);

        int i = 0;
        int currentIndex = 0;
        for (auto& block : this->simulationBlocks)
        {
            std::shared_ptr<ISimulationBlockWithContinuousStates> blockWithContinuousStates = std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block);
            if (blockWithContinuousStates)
            {
                std::vector<double> derivatives_i = blockWithContinuousStates->GetContinuousStateDerivatives(sampleTime, currentTime);
                for (int j = 0; j < derivatives_i.size(); j++)
                {
                    derivatives[currentIndex] = derivatives_i[j];
                    currentIndex++;
                }
            }

            i += 1;
        }

        return derivatives;
    }



    std::vector<std::vector<double>> BasicOdeSolver::GetJacobian(std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        std::vector<std::vector<double>> jacobian(this->totalStates, std::vector<double>(this->totalStates, 0.0));

        std::vector<double> originalDerivatives = this->GetDerivatives(sampleTime, currentTime);
        std::vector<double> originalStates = this->GetStates();

        for (int i = 0; i < this->totalStates; i++)
        {
            std::vector<double> states = originalStates;
            double insertedPerturbation;
            if (states[i] == 0.0)
            {
                insertedPerturbation = 1e-6;
                states[i] = insertedPerturbation;
            }
            else
            {
                insertedPerturbation = states[i] * 0.01;
                states[i] += insertedPerturbation;
            }

            this->SetStates(states);
            std::vector<double> derivativesPerturbed = this->GetDerivatives(sampleTime, currentTime);
            for (int j = 0; j < this->totalStates; j++)
            {
                jacobian[j][i] = (derivativesPerturbed[j] - originalDerivatives[j]) / insertedPerturbation;
            }
        }
        this->SetStates(originalStates);

        return jacobian;
    }

    const std::vector<std::pair<double, double>> BasicOdeSolver::GetEvents(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double eventTime, std::vector<double> eventTimeStates) const
    {
        spdlog::get("default_pysyslink")->debug("Looking for events...");
        std::vector<std::pair<double, double>> events = {};

        int currentIndex = 0;
        int i = 0;
        for (auto& block : this->simulationBlocks)
        {
            std::vector<double>::const_iterator first = eventTimeStates.begin() + currentIndex;
            std::vector<double>::const_iterator last = eventTimeStates.begin() + currentIndex + this->continuousStatesInEachBlock[i];
            std::vector<double> eventTimeStates_i(first, last);
            std::vector<std::pair<double, double>> events_i = block->GetEvents(sampleTime, eventTime, eventTimeStates_i);
            for (int j = 0; j < events_i.size(); j++)
            {
                events.push_back(events_i[j]);
            }
            currentIndex += this->continuousStatesInEachBlock[i];

            i += 1;
        }
        return events;
    }

    void BasicOdeSolver::SetStates(std::vector<double> newStates)
    {
        int currentIndex = 0;
        int i = 0;
        for (auto& block : this->simulationBlocks)
        {
            std::shared_ptr<ISimulationBlockWithContinuousStates> blockWithContinuousStates = std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block);
            if (blockWithContinuousStates)
            {
                std::vector<double>::const_iterator first = newStates.begin() + currentIndex;
                std::vector<double>::const_iterator last = newStates.begin() + currentIndex + this->continuousStatesInEachBlock[i];
                std::vector<double> newStates_i(first, last);
                blockWithContinuousStates->SetContinuousStates(newStates_i);

                currentIndex += this->continuousStatesInEachBlock[i];
            }

            i += 1;
        }
    }


    void BasicOdeSolver::ComputeBlockOutputs(std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime, bool isMinorStep)
    {
        block->ComputeOutputsOfBlock(sampleTime, currentTime, isMinorStep);
        for (int i = 0; i < block->GetOutputPorts().size(); i++)
        {
            for (auto& connectedPort : simulationModel->GetConnectedPorts(block, i))
            {
                block->GetOutputPorts()[i]->TryCopyValueToPort(*connectedPort);
            }
        }
    }

    std::vector<double> BasicOdeSolver::SystemModel(std::vector<double> states, double time)
    {
        this->SetStates(states);
        this->ComputeMinorOutputs(this->sampleTime, time);
        return this->GetDerivatives(this->sampleTime, time);
    }
    
    std::vector<std::vector<double>> BasicOdeSolver::SystemModelJacobian(std::vector<double> states, double time)
    {
        this->SetStates(states);
        this->ComputeMinorOutputs(this->sampleTime, time);
        return this->GetJacobian(this->sampleTime, time);
    }

    void BasicOdeSolver::UpdateStatesToNextTimeHits()
    {
        if (this->nextTimeHitStates.size() != 0)
        {
            this->SetStates(this->nextTimeHitStates);
        }
    }

    std::tuple<bool, std::vector<double>, double> BasicOdeSolver::OdeStepSolverStep(std::function<std::vector<double>(std::vector<double>, double)> systemLambda, 
                                            std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobianLambda,
                                            std::vector<double> states_0, double currentTime, double timeStep)
    {
        std::tuple<bool, std::vector<double>, double> result;
        if (this->odeStepSolver->IsJacobianNeeded())
        {
            spdlog::get("default_pysyslink")->debug("Jacobian needed");
            result = this->odeStepSolver->SolveStep(systemLambda, systemJacobianLambda, this->GetStates(), currentTime, timeStep);
        }
        else
        {
            spdlog::get("default_pysyslink")->debug("Jacobian not needed");
            result = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, timeStep);
        }
        return result;
    }


    void BasicOdeSolver::DoStep(double currentTime, double timeStep)
    {
        auto systemLambda = [this](std::vector<double> states, double time) {
            return this->SystemModel(states, time);
        };

        auto systemJacobianLambda = [this](std::vector<double> states, double time) {
            return this->SystemModelJacobian(states, time);
        };

        this->UpdateStatesToNextTimeHits();

        spdlog::get("default_pysyslink")->debug("Requested step size in time {}: {}", currentTime, timeStep);

        if (this->currentKnownTimeHit < this->knownTimeHits.size())
        {
            if (currentTime == this->knownTimeHits[this->currentKnownTimeHit])
            {
                this->currentKnownTimeHit += 1;
            }
            else if (currentTime > this->knownTimeHits[this->currentKnownTimeHit])
            {
                throw std::runtime_error("Current time is " + std::to_string(currentTime) + " but a known time hit should have already been resolved: " + std::to_string(this->knownTimeHits[this->currentKnownTimeHit]));
            }
            if ((currentTime + timeStep) > this->knownTimeHits[this->currentKnownTimeHit])
            {
                timeStep = this->knownTimeHits[this->currentKnownTimeHit] - currentTime;
                spdlog::get("default_pysyslink")->debug("A step size was requested, but a known time hit had to be solved before. New proposed time step: {}", timeStep);
            }
        }

        std::vector<std::pair<double, double>> initialEvents = this->GetEvents(this->sampleTime, currentTime, this->GetStates());
        
        double appliedTimeStep = timeStep;

        std::tuple<bool, std::vector<double>, double> result = this->OdeStepSolverStep(systemLambda, systemJacobianLambda, this->GetStates(), currentTime, appliedTimeStep);

        double newSuggestedTimeStep = std::get<2>(result);
        while (!std::get<0>(result))
        {
            spdlog::get("default_pysyslink")->debug("Step with size: {} rejected, trying new suggested step size; {}", appliedTimeStep, newSuggestedTimeStep);
            appliedTimeStep = newSuggestedTimeStep;
            result = this->OdeStepSolverStep(systemLambda, systemJacobianLambda, this->GetStates(), currentTime, newSuggestedTimeStep);
            newSuggestedTimeStep = std::get<2>(result);
        }

        this->nextTimeHitStates = std::get<1>(result);

        if (this->activateEvents)
        {
            std::vector<std::pair<double, double>> currentEvents = this->GetEvents(this->sampleTime, currentTime + appliedTimeStep, nextTimeHitStates);
            bool isThereEvent = false;
            for (int i = 0; i < currentEvents.size(); i++)
            {
                spdlog::get("default_pysyslink")->debug("Current event {}: {}", i, currentEvents[i].first);
                spdlog::get("default_pysyslink")->debug("Initial event {}: {}", i, initialEvents[i].first);
                if ((initialEvents[i].first < 0) != (currentEvents[i].first < 0))
                {
                    isThereEvent = true;
                }
            }

            if (isThereEvent)
            {
                double t_1 = currentTime;
                double t_2 = currentTime + appliedTimeStep;
                spdlog::get("default_pysyslink")->debug("Event happened on interval {} - {}", t_1, t_2);

                while ((t_2 - t_1) > this->eventTolerance)
                {
                    auto currentIntervalCenterResult = this->OdeStepSolverStep(systemLambda, systemJacobianLambda, this->GetStates(), currentTime, (t_2+t_1)/2 - currentTime);

                    std::vector<std::pair<double, double>> currentIntervalCenterEvents = this->GetEvents(this->sampleTime, (t_2+t_1)/2, std::get<1>(currentIntervalCenterResult));
                    bool isThereEventInCurrentIntervalHalf = false;
                    for (int i = 0; i < currentIntervalCenterEvents.size(); i++)
                    {
                        spdlog::get("default_pysyslink")->debug("Initial event i: {}. Current event i: {}", initialEvents[i].first, currentIntervalCenterEvents[i].first);
                        if ((initialEvents[i].first < 0) != (currentIntervalCenterEvents[i].first < 0))
                        {
                            isThereEventInCurrentIntervalHalf = true;
                        }
                    }
                    if (isThereEventInCurrentIntervalHalf)
                    {
                        spdlog::get("default_pysyslink")->debug("Event happened on interval {} - {}, reduce t_2 in {}", t_1, (t_2+t_1)/2, (t_2-t_1)/2);
                                    
                        t_2 -= (t_2-t_1)/2;
                    }
                    else
                    {
                        spdlog::get("default_pysyslink")->debug("No event on interval {} - {}, increase t_1 in {}", t_1, (t_2+t_1)/2, (t_2-t_1)/2);
                        t_1 += (t_2-t_1)/2;
                    }
                }
                auto eventResolutionTimeResult = this->OdeStepSolverStep(systemLambda, systemJacobianLambda, this->GetStates(), currentTime, t_2 - currentTime);

                appliedTimeStep = t_2 - currentTime;
                this->nextTimeHitStates = std::get<1>(eventResolutionTimeResult);
                newSuggestedTimeStep = std::get<2>(eventResolutionTimeResult);

                spdlog::get("default_pysyslink")->debug("Event resolved, new time hit: {}", t_2);
            }
        }
        spdlog::get("default_pysyslink")->debug("Applied step size in time {}: {}", currentTime, appliedTimeStep);

        this->nextSuggestedTimeStep = newSuggestedTimeStep;
        this->nextUnknownTimeHit = currentTime + appliedTimeStep;
    }

    double BasicOdeSolver::GetNextTimeHit() const
    {
        if (this->currentKnownTimeHit < this->knownTimeHits.size())
        {
            return std::min(this->nextUnknownTimeHit, this->knownTimeHits[this->currentKnownTimeHit]);
        }
        else
        {
            return this->nextUnknownTimeHit;
        }
    }

    double BasicOdeSolver::GetNextSuggestedTimeStep() const
    {
        return this->nextSuggestedTimeStep;
    }
} // namespace PySysLinkBase
