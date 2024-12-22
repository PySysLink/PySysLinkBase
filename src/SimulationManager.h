#ifndef SRC_SIMULATION_MANAGER
#define SRC_SIMULATION_MANAGER

#include "SimulationModel.h"
#include "SimulationOptions.h"

namespace PySysLinkBase
{
    class SimulationManager
    {
        public:
        void RunSimulation(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<SimulationOptions> simulationOptions);

        private:
        void ClasificateBlocks(std::vector<std::shared_ptr<PySysLinkBase::ISimulationBlock>> orderedBlocks, 
                            std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>>& blocksForEachDiscreteSampleTime,
                            std::vector<std::shared_ptr<ISimulationBlock>>& blocksWithConstantSampleTime);
    
        void ProcessBlock(std::shared_ptr<SimulationModel> simulationModel, std::shared_ptr<ISimulationBlock> block, std::shared_ptr<SampleTime> sampleTime, double currentTime);

        std::map<double, std::vector<std::shared_ptr<SampleTime>>> GetTimeHitsToSampleTimes(std::shared_ptr<SimulationOptions> simulationOptions, std::map<std::shared_ptr<SampleTime>, std::vector<std::shared_ptr<ISimulationBlock>>> blocksForEachDiscreteSampleTime);


    };
}

#endif /* SRC_SIMULATION_MANAGER */
