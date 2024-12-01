#ifndef SRC_PY_SYS_LINK_BASE_SIMULATION_MODEL
#define SRC_PY_SYS_LINK_BASE_SIMULATION_MODEL

#include <vector>
#include "ISimulationBlock.h"
#include "PortLink.h"

namespace PySysLinkBase
{
    class SimulationModel
    {
    public:
        std::vector<std::unique_ptr<ISimulationBlock>> simulationBlocks;
        std::vector<std::unique_ptr<PortLink>> portLinks;
        
        SimulationModel(std::vector<std::unique_ptr<ISimulationBlock>> simulationBlocks, std::vector<std::unique_ptr<PortLink>> portLinks);

        
    };
}

#endif /* SRC_PY_SYS_LINK_BASE_SIMULATION_MODEL */
