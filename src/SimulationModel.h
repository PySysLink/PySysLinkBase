#ifndef SRC_PY_SYS_LINK_BASE_SIMULATION_MODEL
#define SRC_PY_SYS_LINK_BASE_SIMULATION_MODEL

#include <vector>
#include "ISimulationBlock.h"
#include "PortLink.h"
#include "PortsAndSignalValues/InputPort.h"
#include "PortsAndSignalValues/OutputPort.h"
#include <optional>

namespace PySysLinkBase
{
    class SimulationModel
    {
    public:
        std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks;
        std::vector<std::shared_ptr<PortLink>> portLinks;
        
        SimulationModel(std::vector<std::unique_ptr<ISimulationBlock>> simulationBlocks, std::vector<std::unique_ptr<PortLink>> portLinks);

        const std::vector<std::shared_ptr<InputPort>> GetConnectedPorts(const std::shared_ptr<OutputPort> origin) const;
        const std::shared_ptr<ISimulationBlock> GetOriginBlock(const std::shared_ptr<InputPort> sinkPort) const;

        const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> GetDirectBlockChains();

        const std::vector<std::shared_ptr<ISimulationBlock>> OrderBlockChainsOntoFreeOrder(const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> directBlockChains);
        
        void PropagateSampleTimes();

    private:
        const std::vector<std::shared_ptr<ISimulationBlock>> GetFreeSourceBlocks();

        std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> GetDirectBlockChainsOfSourceBlock(std::shared_ptr<ISimulationBlock> freeSourceBlock);
        
        void FindChains(std::shared_ptr<ISimulationBlock> currentBlock, std::vector<std::shared_ptr<ISimulationBlock>> currentChain, std::vector<std::vector<std::shared_ptr<ISimulationBlock>>>& resultChains);
    };
}

#endif /* SRC_PY_SYS_LINK_BASE_SIMULATION_MODEL */
