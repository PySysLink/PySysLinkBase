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

        double RunSimulationStep();
        std::shared_ptr<SimulationOutput> GetSimulationOutput();

        private:
        bool hasRunFullSimulation = false;
        bool isRunningStepByStep = false;
        bool isFirstStepDone = false;
        std::vector<std::shared_ptr<SampleTime>> nextSampleTimesToProcess = {};

        void ClassifyBlocks(std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks, 
                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachDiscreteSampleTime,
                            std::vector<std::shared_ptr<ISimulationBlock>>& blocksWithConstantSampleTime,
                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachContinuousSampleTimeGroup);
    
        void ProcessBlock(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime, bool isMinorStep=false);

        void GetTimeHitsToSampleTimes(std::shared_ptr<SimulationOptions> simulationOptions, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime);

        std::tuple<double, int, std::vector<std::shared_ptr<SampleTime>>> GetNearestTimeHit(int nextDiscreteTimeHitToProcessIndex);
        std::tuple<double, std::vector<std::shared_ptr<SampleTime>>> GetNearestTimeHit(double currentTime);


        std::map<std::shared_ptr<SampleTime>, std::shared_ptr<BasicOdeSolver>> odeSolversForEachContinuousSampleTimeGroup;

        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime;
        std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachContinuousSampleTimeGroup;
        std::vector<std::shared_ptr<ISimulationBlock>> blocksWithConstantSampleTime;

        bool IsBlockInSampleTimes(const std::shared_ptr<ISimulationBlock>& block, const std::vector<std::shared_ptr<SampleTime>>& sampleTimes, 
                                            const std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blockMap);

        std::map<double, std::vector<std::shared_ptr<SampleTime>>> timeHitsToSampleTimes;
        std::vector<double> timeHits;
        double currentTime;

        std::shared_ptr<SimulationModel> simulationModel;
        std::shared_ptr<SimulationOptions> simulationOptions;

        std::shared_ptr<SimulationOutput> simulationOutput;

        void ValueUpdateBlockEventCallback(const std::shared_ptr<ValueUpdateBlockEvent> blockEvent);

        void LogSignalInputReadCallback(const std::string blockId, const std::vector<std::shared_ptr<PySysLinkBase::InputPort>> inputPorts, int inputPortIndex, std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime);
        void LogSignalOutputUpdateCallback(const std::string blockId, const std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts, int outputPortIndex, std::shared_ptr<PySysLinkBase::SampleTime> sampleTime, double currentTime);
        void UpdateConfigurationValueCallback(const std::string blockId, const std::string keyName, ConfigurationValue value);

        std::unordered_map<const Port*, const Port*> portToLogInToAvoidRepetition = {};
        std::unordered_map<const Port*, std::pair<std::string, int>> loggedPortToCorrespondentBlockIdAndOutputPortIndex = {};

        std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks;

        void ProcessBlocksInSampleTimes(const std::vector<std::shared_ptr<SampleTime>> sampleTimes, bool isMinorStep=false);
        void MakeFirstSimulationStep();
        void ProcessTimeHit(double time, const std::vector<std::shared_ptr<SampleTime>>& sampleTimesToProcess);

        std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocksForceOutputUpdate = {};
    };
}

#endif /* SRC_SIMULATION_MANAGER */
