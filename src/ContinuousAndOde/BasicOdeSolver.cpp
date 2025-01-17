#include "BasicOdeSolver.h"
#include <limits>
#include <spdlog/spdlog.h>

namespace PySysLinkBase
{
    BasicOdeSolver::BasicOdeSolver(std::shared_ptr<IOdeStepSolver> odeStepSolver, std::shared_ptr<SimulationModel> simulationModel, 
                                    std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::shared_ptr<SampleTime> sampleTime, double firstTimeStep) 
                                    : odeStepSolver(odeStepSolver), simulationModel(simulationModel), simulationBlocks(simulationBlocks), sampleTime(sampleTime), firstTimeStep(firstTimeStep)
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
            spdlog::get("default_pysyslink")->debug("Is block continuous?: {}", block->GetId());

            std::shared_ptr<ISimulationBlockWithContinuousStates> blockWithContinuousStates = std::dynamic_pointer_cast<ISimulationBlockWithContinuousStates>(block);
            if (blockWithContinuousStates)
            {
                spdlog::get("default_pysyslink")->debug("Block with continuous states: {}", blockWithContinuousStates->GetId());
                std::vector<double> states_i = blockWithContinuousStates->GetContinuousStates();
                for (int j = 0; j < states_i.size(); j++)
                {
                    states[currentIndex] = states_i[j];
                    currentIndex++;
                }
            }

            i += 1;
        }
        
        spdlog::get("default_pysyslink")->debug("States size: {}", states.size());

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
        
        auto result = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, timeStep);
        double newSuggestedTimeStep = std::get<2>(result);
        while (!std::get<0>(result))
        {
            spdlog::get("default_pysyslink")->debug("Step rejected, trying new suggested step size");
            result = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, newSuggestedTimeStep);
            newSuggestedTimeStep = std::get<2>(result);
        }
        this->SetStates(std::get<1>(result));

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
