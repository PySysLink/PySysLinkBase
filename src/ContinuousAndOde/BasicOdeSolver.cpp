#include "BasicOdeSolver.h"
#include <limits>
#include <spdlog/spdlog.h>

namespace PySysLinkBase
{
    BasicOdeSolver::BasicOdeSolver(std::shared_ptr<IOdeStepSolver> odeStepSolver, std::shared_ptr<SimulationModel> simulationModel, 
                                    std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::shared_ptr<SampleTime> sampleTime, 
                                    double firstTimeStep, bool activateEvents, double eventTolerance) 
                                    : odeStepSolver(odeStepSolver), simulationModel(simulationModel), simulationBlocks(simulationBlocks), sampleTime(sampleTime), firstTimeStep(firstTimeStep),
                                    activateEvents(activateEvents), eventTolerance(eventTolerance)
    {
        this->continuousStatesInEachBlock = {};
        this->totalStates = 0;
        this->nextTimeHit = std::numeric_limits<double>::quiet_NaN();
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
        }

        this->nextTimeHitStates = {};
    }

    void BasicOdeSolver::ComputeMinorOutputs(std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        for (auto& block : this->simulationBlocks)
        {
            this->ComputeBlockOutputs(block, sampleTime, currentTime, true);
        }
    }

    void BasicOdeSolver::ComputeMajorOutputs(std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        for (auto& block : this->simulationBlocks)
        {
            this->ComputeBlockOutputs(block, sampleTime, currentTime, false);
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
                std::vector<double> derivatives_i = blockWithContinuousStates->GetContinousStateDerivatives(sampleTime, currentTime);
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

    const std::vector<std::pair<double, double>> BasicOdeSolver::GetEvents(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double eventTime, std::vector<double> eventTimeStates) const
    {
        std::vector<std::pair<double, double>> events = {};

        int currentIndex = 0;
        int i = 0;
        for (auto& block : this->simulationBlocks)
        {
            std::shared_ptr<ISimulationBlockWithContinuousStates> blockWithContinuousStates = std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block);
            if (blockWithContinuousStates)
            {
                std::vector<double>::const_iterator first = eventTimeStates.begin() + currentIndex;
                std::vector<double>::const_iterator last = eventTimeStates.begin() + currentIndex + this->continuousStatesInEachBlock[i];
                std::vector<double> eventTimeStates_i(first, last);
                std::vector<std::pair<double, double>> events_i = blockWithContinuousStates->GetEvents(sampleTime, eventTime, eventTimeStates_i);
                for (int j = 0; j < events_i.size(); j++)
                {
                    events.push_back(events_i[j]);
                }
                currentIndex += this->continuousStatesInEachBlock[i];
            }

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


    void BasicOdeSolver::DoStep(double currentTime, double timeStep)
    {
        auto systemLambda = [this](std::vector<double> states, double time) {
            return this->SystemModel(states, time);
        };

        if (this->nextTimeHitStates.size() != 0)
        {
            this->SetStates(this->nextTimeHitStates);
        }

        std::vector<std::pair<double, double>> initialEvents = this->GetEvents(this->sampleTime, currentTime, this->GetStates());
        
        double appliedTimeStep = timeStep;
        auto result = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, timeStep);

        double newSuggestedTimeStep = std::get<2>(result);
        while (!std::get<0>(result))
        {
            spdlog::get("default_pysyslink")->debug("Step rejected, trying new suggested step size");
            appliedTimeStep = newSuggestedTimeStep;
            result = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, newSuggestedTimeStep);
            newSuggestedTimeStep = std::get<2>(result);
        }

        this->nextTimeHitStates = std::get<1>(result);

        if (this->activateEvents)
        {
            std::vector<std::pair<double, double>> currentEvents = this->GetEvents(this->sampleTime, currentTime + appliedTimeStep, nextTimeHitStates);
            bool isThereEvent = false;
            for (int i = 0; i < currentEvents.size(); i++)
            {
                if ((initialEvents[i].first < 0) != (currentEvents[i].first < 0))
                {
                    isThereEvent = true;
                }
            }

            if (isThereEvent)
            {
                double t_1 = currentTime;
                double t_2 = currentTime + appliedTimeStep;

                while ((t_2 - t_1) > this->eventTolerance)
                {
                    auto currentIntervalCenterResult = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, (t_2+t_1)/2 - currentTime);

                    std::vector<std::pair<double, double>> currentIntervalCenterEvents = this->GetEvents(this->sampleTime, (t_2+t_1)/2, std::get<1>(currentIntervalCenterResult));
                    bool isThereEventInCurrentIntervalHalf = false;
                    for (int i = 0; i < currentEvents.size(); i++)
                    {
                        if ((initialEvents[i].first < 0) != (currentEvents[i].first < 0))
                        {
                            isThereEventInCurrentIntervalHalf = true;
                        }
                    }
                    if (isThereEventInCurrentIntervalHalf)
                    {
                        t_2 -= (t_2-t_1)/2;
                    }
                    else
                    {
                        t_1 += (t_2-t_1)/2;
                    }
                }

                newSuggestedTimeStep = (t_2+t_1)/2 - currentTime;

            }
        }
            
        this->nextSuggestedTimeStep = newSuggestedTimeStep;
        this->nextTimeHit = currentTime + newSuggestedTimeStep;

        this->ComputeMajorOutputs(this->sampleTime, currentTime);
    }

    double BasicOdeSolver::GetNextTimeHit() const
    {
        return this->nextTimeHit;
    }

    double BasicOdeSolver::GetNextSuggestedTimeStep() const
    {
        return this->nextSuggestedTimeStep;
    }
} // namespace PySysLinkBase
