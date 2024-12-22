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
    };
}

#endif /* SRC_SIMULATION_MANAGER */
