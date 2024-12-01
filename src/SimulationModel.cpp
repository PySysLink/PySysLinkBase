#include "SimulationModel.h"

namespace PySysLinkBase
{  
    SimulationModel::SimulationModel(std::vector<std::unique_ptr<ISimulationBlock>> simulationBlocks, std::vector<std::unique_ptr<PortLink>> portLinks) 
    {
        this->simulationBlocks = std::move(simulationBlocks);
        this->portLinks = std::move(portLinks);
    }
} // namespace PySysLinkBase
