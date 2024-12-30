#ifndef SRC_BASIC_ODE_SOLVER
#define SRC_BASIC_ODE_SOLVER

#include "IOdeSolver.h"
#include "IOdeStepSolver.h"
#include "ISimulationBlockWithContinuousStates.h"
#include "SimulationModel.h"
#include <memory>
#include <vector>

namespace PySysLinkBase
{
    class BasicOdeSolver : public IOdeSolver
    {
        private:
            std::shared_ptr<IOdeStepSolver> odeStepSolver;
            std::shared_ptr<SimulationModel> simulationModel;
            std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks;
            std::vector<int> continuousStatesInEachBlock;
            int totalStates;

            std::shared_ptr<SampleTime> sampleTime;
            
            void ComputeBlockOutputs(std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime);
            void ComputeMinorOutputs(std::shared_ptr<SampleTime> sampleTime, double currentTime);
            std::vector<double> GetDerivatives(std::shared_ptr<SampleTime> sampleTime, double currentTime);
            void SetStates(std::vector<double> newStates);
            std::vector<double> GetStates();
        public:
            std::vector<double> SystemModel(std::vector<double> states, double time);

            BasicOdeSolver(std::shared_ptr<IOdeStepSolver> odeStepSolver, std::shared_ptr<SimulationModel> simulationModel, 
                            std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::shared_ptr<SampleTime> sampleTime);
            double DoStep(std::shared_ptr<SampleTime> sampleTime, double currentTime, double timeStep);
    };
} // namespace PySysLinkBase


#endif /* SRC_BASIC_ODE_SOLVER */
