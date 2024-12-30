#include "BasicOdeSolver.h"

namespace PySysLinkBase
{
    BasicOdeSolver::BasicOdeSolver(std::shared_ptr<IOdeStepSolver> odeStepSolver, std::shared_ptr<SimulationModel> simulationModel, 
                                    std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::shared_ptr<SampleTime> sampleTime) 
                                    : odeStepSolver(odeStepSolver), simulationModel(simulationModel), simulationBlocks(simulationBlocks), sampleTime(sampleTime)
    {
        this->continuousStatesInEachBlock = {};
        this->totalStates = 0;

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
            this->ComputeBlockOutputs(block, sampleTime, currentTime);
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


    void BasicOdeSolver::ComputeBlockOutputs(std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime)
    {
        block->ComputeOutputsOfBlock(sampleTime, currentTime);
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


    double BasicOdeSolver::DoStep(std::shared_ptr<SampleTime> sampleTime, double currentTime, double timeStep)
    {
        auto systemLambda = [this](std::vector<double> states, double time) {
            return this->SystemModel(states, time);
        };

        auto resoult = this->odeStepSolver->SolveStep(systemLambda, this->GetStates(), currentTime, timeStep);
        this->SetStates(std::get<0>(resoult));

        return std::get<1>(resoult);
    }
} // namespace PySysLinkBase
