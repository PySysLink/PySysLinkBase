#ifndef SRC_SIMULATION_MANAGER
#define SRC_SIMULATION_MANAGER

#include "SimulationModel.h"

namespace PySysLinkBase
{
    class SimulationManager
    {
        public:
        void RunSimulation(std::shared_ptr<SimulationModel> simulationModel, double startTime, double endTime);
    };
}

#endif /* SRC_SIMULATION_MANAGER */
