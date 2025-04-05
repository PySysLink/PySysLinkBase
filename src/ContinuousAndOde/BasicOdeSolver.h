#ifndef SRC_BASIC_ODE_SOLVER
#define SRC_BASIC_ODE_SOLVER

#include "IOdeStepSolver.h"
#include "ISimulationBlockWithContinuousStates.h"
#include "../SimulationModel.h"
#include <memory>
#include <vector>
#include "../SimulationOptions.h"

namespace PySysLinkBase
{

    class BasicOdeSolver
    {
        private:
            std::shared_ptr<IOdeStepSolver> odeStepSolver;
            std::shared_ptr<SimulationModel> simulationModel;
            std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks;
            std::vector<int> continuousStatesInEachBlock;
            int totalStates;

            std::vector<double> knownTimeHits = {};
            int currentKnownTimeHit = 0;
            double nextUnknownTimeHit;
            double nextSuggestedTimeStep;
            std::vector<double> nextTimeHitStates;

            std::shared_ptr<SampleTime> sampleTime;
            
            void ComputeBlockOutputs(std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime, bool isMinorStep=false);
            void ComputeMinorOutputs(std::shared_ptr<SampleTime> sampleTime, double currentTime);
            std::vector<double> GetDerivatives(std::shared_ptr<SampleTime> sampleTime, double currentTime);
            std::vector<std::vector<double>> GetJacobian(std::shared_ptr<SampleTime> sampleTime, double currentTime);
            void SetStates(std::vector<double> newStates);
            std::vector<double> GetStates();

            std::tuple<bool, std::vector<double>, double> OdeStepSolverStep(std::function<std::vector<double>(std::vector<double>, double)> systemLambda, 
                                                    std::function<std::vector<std::vector<double>>(std::vector<double>, double)> systemJacobianLambda,
                                                    std::vector<double> states_0, double currentTime, double timeStep);

            bool activateEvents;
            double eventTolerance;
            const std::vector<std::pair<double, double>> GetEvents(const std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double eventTime, std::vector<double> eventTimeStates) const;
        public:
            double firstTimeStep;

            std::vector<double> SystemModel(std::vector<double> states, double time);
            std::vector<std::vector<double>> SystemModelJacobian(std::vector<double> states, double time);

            BasicOdeSolver(std::shared_ptr<IOdeStepSolver> odeStepSolver, std::shared_ptr<SimulationModel> simulationModel, 
                            std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::shared_ptr<SampleTime> sampleTime, 
                            std::shared_ptr<SimulationOptions> simulationOptions,
                            double firstTimeStep = 1e-6, bool activateEvents=true, double eventTolerance=1e-2);
            
            void UpdateStatesToNextTimeHits();
            void DoStep(double currentTime, double timeStep);
            void ComputeMajorOutputs(double currentTime);

            double GetNextTimeHit() const;
            double GetNextSuggestedTimeStep() const;
    };
} // namespace PySysLinkBase


#endif /* SRC_BASIC_ODE_SOLVER */
