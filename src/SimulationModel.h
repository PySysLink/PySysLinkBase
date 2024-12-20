#ifndef SRC_SIMULATION_MODEL
#define SRC_SIMULATION_MODEL

#include <vector>
#include "ISimulationBlock.h"
#include "PortLink.h"
#include "PortsAndSignalValues/InputPort.h"
#include "PortsAndSignalValues/OutputPort.h"
#include <optional>
#include "IBlockEventsHandler.h"

namespace PySysLinkBase
{
    class SimulationModel
    {
    public:
        std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks;
        std::vector<std::shared_ptr<PortLink>> portLinks;
        std::shared_ptr<IBlockEventsHandler> blockEventsHandler;
        
        SimulationModel(std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::vector<std::shared_ptr<PortLink>> portLinks, std::shared_ptr<IBlockEventsHandler> blockEventsHandler);

        const std::vector<std::shared_ptr<InputPort>> GetConnectedPorts(const std::shared_ptr<ISimulationBlock> originBlock, int outputPortIndex) const;
        const std::pair<std::vector<std::shared_ptr<ISimulationBlock>>, std::vector<int>> GetConnectedBlocks(const std::shared_ptr<ISimulationBlock> originBlock, int outputPortIndex) const;
        const std::shared_ptr<ISimulationBlock> GetOriginBlock(const std::shared_ptr<ISimulationBlock> sinkBlock, int inputPortIndex) const;

        const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> GetDirectBlockChains();

        const std::vector<std::shared_ptr<ISimulationBlock>> OrderBlockChainsOntoFreeOrder(const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> directBlockChains);
        
        void PropagateSampleTimes();

    private:
        const std::vector<std::shared_ptr<ISimulationBlock>> GetFreeSourceBlocks();

        std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> GetDirectBlockChainsOfSourceBlock(std::shared_ptr<ISimulationBlock> freeSourceBlock);
        
        void FindChains(std::shared_ptr<ISimulationBlock> currentBlock, std::vector<std::shared_ptr<ISimulationBlock>> currentChain, std::vector<std::vector<std::shared_ptr<ISimulationBlock>>>& resultChains);
    };
}

#endif /* SRC_SIMULATION_MODEL */
