#ifndef SRC_SIMULATION_MANAGER
#define SRC_SIMULATION_MANAGER

#include "SimulationModel.h"
#include "SimulationOptions.h"
#include "ContinuousAndOde/BasicOdeSolver.h"
#include "ContinuousAndOde/IOdeStepSolver.h"
#include "SimulationOutput.h"
#include "BlockEvents/ValueUpdateBlockEvent.h"

#include <tuple>
#include <unordered_map>
#include <functional>

namespace PySysLinkBase
{
    class SimulationManager
    {
        public:
        SimulationManager(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<SimulationOptions> simulationOptions);
        std::shared_ptr<SimulationOutput> RunSimulation();

        private:

        void ClasificateBlocks(std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks, 
                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachDiscreteSampleTime,
                            std::vector<std::shared_ptr<ISimulationBlock>>& blocksWithConstantSampleTime,
                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachContinuousSampleTimeGroup);
    
        void ProcessBlock(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime);

        void GetTimeHitsToSampleTimes(std::shared_ptr<SimulationOptions> simulationOptions, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime);

        std::tuple<double, int, std::vector<std::shared_ptr<SampleTime>>> GetNearestTimeHit(int nextDiscreteTimeHitToProcessIndex);


        std::map<std::shared_ptr<SampleTime>, std::shared_ptr<BasicOdeSolver>> odeSolversForEachContinuousSampleTimeGroup;

        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime;
        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachContinuousSampleTimeGroup;
        std::vector<std::shared_ptr<ISimulationBlock>> blocksWithConstantSampleTime;

        std::map<double, std::vector<std::shared_ptr<SampleTime>>> timeHitsToSampleTimes;
        std::vector<double> timeHits;
        double currentTime;

        std::shared_ptr<SimulationModel> simulationModel;
        std::shared_ptr<SimulationOptions> simulationOptions;

        std::shared_ptr<SimulationOutput> simulationOutput;

        void ValueUpdateBlockEventCallback(const std::shared_ptr<ValueUpdateBlockEvent> blockEvent);
        
        void LogSignalOutputUpdateCallback(const std::string blockId, const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts, int outputPortIndex, std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime);

        std::unordered_map<const Port*, const Port*> portToLogInToAvoidRepetition = {};
        std::unordered_map<const Port*, std::pair<std::string, int>> loggedPortToCorrespondentBlockIdAndOutputPortIndex = {};

        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks;
    };
}

#endif /* SRC_SIMULATION_MANAGER */
