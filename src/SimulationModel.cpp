#include "SimulationModel.h"
#include <stdexcept>
#include <iostream>
#include <unordered_set>

namespace PySysLinkBase
{  
    SimulationModel::SimulationModel(std::vector<std::unique_ptr<ISimulationBlock>> simulationBlocks, std::vector<std::unique_ptr<PortLink>> portLinks) 
    {
        this->simulationBlocks.insert(this->simulationBlocks.end(), std::make_move_iterator(simulationBlocks.begin()), std::make_move_iterator(simulationBlocks.end()));
        this->portLinks.insert(this->portLinks.end(), std::make_move_iterator(portLinks.begin()), std::make_move_iterator(portLinks.end()));
    }

    const std::vector<std::shared_ptr<InputPort>> SimulationModel::GetConnectedPorts(const std::shared_ptr<OutputPort> origin) const
    {
        std::vector<std::shared_ptr<InputPort>> connectedPorts = {};
        for (int i=0; i < this->portLinks.size(); i++)
        {
            if (this->portLinks[i]->origin == origin)
            {
                connectedPorts.push_back(this->portLinks[i]->sink);
            }
        }
        return connectedPorts;
    }

    const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> SimulationModel::GetDirectBlockChains() 
    {
        std::vector<std::shared_ptr<ISimulationBlock>> freeSourceBlocks = this->GetFreeSourceBlocks();
        std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> directBlockChains = {};

        for (int i = 0; i < freeSourceBlocks.size(); i++)
        {
            std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> directBlockChains_i = this->GetDirectBlockChainsOfSourceBlock(freeSourceBlocks[i]);
            directBlockChains.insert(directBlockChains.end(), directBlockChains_i.begin(), directBlockChains_i.end());
        }
        return directBlockChains;
    }

    const std::vector<std::shared_ptr<ISimulationBlock>> SimulationModel::GetFreeSourceBlocks()
    {
        std::vector<std::shared_ptr<ISimulationBlock>> freeSourceBlocks = {};
        for (int i = 0; i < this->simulationBlocks.size(); i++)
        {
            if (this->simulationBlocks[i]->IsBlockFreeSource())
            {
                freeSourceBlocks.push_back(this->simulationBlocks[i]);
            }
        }
        return freeSourceBlocks;
    }

    std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> SimulationModel::GetDirectBlockChainsOfSourceBlock(std::shared_ptr<ISimulationBlock> freeSourceBlock) {

        // Initialize the result chains and start the recursion
        std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> resultChains = {};
        this->FindChains(freeSourceBlock, {}, resultChains);

        return resultChains;
    }

    void SimulationModel::FindChains(std::shared_ptr<ISimulationBlock> currentBlock, std::vector<std::shared_ptr<ISimulationBlock>> currentChain, std::vector<std::vector<std::shared_ptr<ISimulationBlock>>>& resultChains)
    {
        // Add the current block to the chain
        currentChain.push_back(currentBlock);

        std::vector<std::shared_ptr<PySysLinkBase::OutputPort>> outputPorts = currentBlock->GetOutputPorts();
        if (outputPorts.empty()) 
        {
            // End the chain if the block has no outputs
            std::cout << "empty end" << std::endl;
            resultChains.push_back(currentChain);
            return;
        }

        // Iterate over all output ports
        for (const auto& outputPort : outputPorts) 
        {
            // Get the input ports connected to this output
            const std::vector<std::shared_ptr<PySysLinkBase::InputPort>> connectedPorts = this->GetConnectedPorts(outputPort);
            if (connectedPorts.empty()) 
            {
                // End the chain if the output port has no connections
                std::cout << "no connection" << std::endl;

                resultChains.push_back(currentChain);
                continue;
            }

            // For each connected input port
            for (const auto& inputPort : connectedPorts) 
            {
                if (inputPort->IsInputDirectBlockChainEnd()) 
                {
                    // End the chain if the connected input port stops direct feedthrough
                    currentChain.push_back(ISimulationBlock::FindBlockById(inputPort->GetOwnerBlock().GetId(), this->simulationBlocks));
                    resultChains.push_back(currentChain);
                    std::cout << "Direct block chain end" << std::endl;
                    continue;
                }

                // Otherwise, continue the chain with the owner block of the connected input port
                this->FindChains(ISimulationBlock::FindBlockById(inputPort->GetOwnerBlock().GetId(), this->simulationBlocks), currentChain, resultChains);
            }
        }
    }

    const std::vector<std::shared_ptr<ISimulationBlock>> SimulationModel::OrderBlockChainsOntoFreeOrder(const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> directBlockChains)
    {
        std::vector<std::shared_ptr<ISimulationBlock>> orderedBlocks;
        std::unordered_set<std::shared_ptr<ISimulationBlock>> processedBlocks;

        // A lambda to check if all feedthrough inputs of a block are processed
        auto areInputsResolved = [&](const std::shared_ptr<ISimulationBlock>& block) -> bool {
            for (const auto& inputPort : block->GetInputPorts()) {
                if (inputPort->HasDirectFeedtrough()) {
                    auto originBlock = GetOriginBlock(inputPort);
                    if (originBlock && processedBlocks.find(originBlock) == processedBlocks.end()) {
                        // If the origin block isn't processed, inputs are not resolved
                        return false;
                    }
                }
            }
            return true;
        };

        // Create a vector of iterators to track progress in each chain
        std::vector<size_t> chainPositions(directBlockChains.size(), 0);

        // Continue until all chains are exhausted
        bool progressMade = true;
        while (progressMade) {
            progressMade = false;

            for (size_t i = 0; i < directBlockChains.size(); ++i) {
                auto& chain = directBlockChains[i];
                size_t& position = chainPositions[i];

                // Process blocks in this chain as far as possible
                while (position < chain.size()) {
                    auto& block = chain[position];
                    if (processedBlocks.find(block) == processedBlocks.end() && areInputsResolved(block)) {
                        // Add the block to the ordered list and mark it as processed
                        orderedBlocks.push_back(block);
                        processedBlocks.insert(block);
                        ++position; // Move to the next block in the chain
                        progressMade = true;
                    } else {
                        // If inputs aren't resolved, stop processing this chain for now
                        break;
                    }
                }
            }
        }
        return orderedBlocks;
    }

    const std::shared_ptr<ISimulationBlock> SimulationModel::GetOriginBlock(const std::shared_ptr<InputPort> sinkPort) const 
    {
        for (const auto& link : portLinks) {
            if (link->sink == sinkPort) {
                return ISimulationBlock::FindBlockById(link->origin->GetOwnerBlock().GetId(), this->simulationBlocks); // Return the block owning the connected output port
            }
        }
        return nullptr; // No connection found
    }

    void SimulationModel::PropagateSampleTimes() {
        // Helper lambdas
        auto hasKnownSampleTime = [](const SampleTime& sampleTime) -> bool {
            return sampleTime.GetSampleTimeType() != SampleTimeType::inherited;
        };

        auto canInheritSampleTime = [](const SampleTime& sampleTime, const SampleTimeType& targetType) -> bool {
            const auto& supportedTypes = sampleTime.GetSupportedSampleTimeTypesForInheritance();
            return std::find(supportedTypes.begin(), supportedTypes.end(), targetType) != supportedTypes.end();
        };

        std::unordered_set<std::shared_ptr<ISimulationBlock>> resolvedBlocks;
        std::unordered_set<std::shared_ptr<ISimulationBlock>> unresolvedBlocks;

        // Initialize the block processing state
        for (const auto& block : simulationBlocks) {
            bool isResolved = true;
            for (const auto& sampleTime : block->GetSampleTimes()) {
                if (!hasKnownSampleTime(sampleTime)) {
                    unresolvedBlocks.insert(block);
                    isResolved = false;
                    break;
                }
            }
            if (isResolved) {
                resolvedBlocks.insert(block);
            }
        }

        // Propagation loop
        bool progressMade = true;
        while (progressMade) {
            progressMade = false;

            // Forward propagation
            for (const auto& block : resolvedBlocks) {
                for (const auto& outputPort : block->GetOutputPorts()) {
                    const auto connectedPorts = GetConnectedPorts(outputPort);
                    for (const auto& inputPort : connectedPorts) {
                        std::shared_ptr<ISimulationBlock> targetBlock = ISimulationBlock::FindBlockById(inputPort->GetOwnerBlock().GetId(), this->simulationBlocks);

                        if (resolvedBlocks.find(targetBlock) != resolvedBlocks.end()) {
                            const auto& outputSampleTimes = block->GetSampleTimes();
                            auto& inputSampleTimes = targetBlock->GetSampleTimes();

                            for (int i = 0; i < inputSampleTimes.size(); ++i) {
                                if (!hasKnownSampleTime(inputSampleTimes[i])) {
                                    for (const auto& outputSampleTime : outputSampleTimes) {
                                        if (canInheritSampleTime(inputSampleTimes[i], outputSampleTime.GetSampleTimeType())) {
                                            inputSampleTimes[i] = outputSampleTime;
                                            progressMade = true;
                                        }
                                    }
                                }
                            }

                            // Check if the target block is now resolved
                            bool isNowResolved = true;
                            for (const auto& sampleTime : inputSampleTimes) {
                                if (!hasKnownSampleTime(sampleTime)) {
                                    isNowResolved = false;
                                    break;
                                }
                            }
                            if (isNowResolved) {
                                resolvedBlocks.insert(targetBlock);
                                unresolvedBlocks.erase(targetBlock);
                            }
                        }
                    }
                }
            }

            // Backward propagation
            for (const auto& block : unresolvedBlocks) {
                const auto& inputPorts = block->GetInputPorts();
                for (const auto& inputPort : inputPorts) {
                    const auto originBlock = GetOriginBlock(inputPort);
                    if (originBlock && resolvedBlocks.find(originBlock) != resolvedBlocks.end()) {
                        auto& blockSampleTimes = block->GetSampleTimes();
                        const auto& originSampleTimes = originBlock->GetSampleTimes();

                        for (size_t i = 0; i < blockSampleTimes.size(); ++i) {
                            if (!hasKnownSampleTime(blockSampleTimes[i])) {
                                for (const auto& originSampleTime : originSampleTimes) {
                                    if (canInheritSampleTime(blockSampleTimes[i], originSampleTime.GetSampleTimeType())) {
                                        blockSampleTimes[i] = originSampleTime;
                                        progressMade = true;
                                    }
                                }
                            }
                        }

                        // Re-check if the block is now resolved
                        bool isNowResolved = true;
                        for (const auto& sampleTime : blockSampleTimes) {
                            if (!hasKnownSampleTime(sampleTime)) {
                                isNowResolved = false;
                                break;
                            }
                        }
                        if (isNowResolved) {
                            resolvedBlocks.insert(block);
                            unresolvedBlocks.erase(block);
                        }
                    }
                }
            }
        }

        // Final validation
        if (!unresolvedBlocks.empty()) {
            throw std::runtime_error("Sample time propagation failed: Unresolved sample times remain.");
        }

        // Optional: Validate compatibility across links
        for (const auto& link : portLinks) {
            const auto& originSampleTimes = link->origin->GetOwnerBlock().GetSampleTimes();
            const auto& sinkSampleTimes = link->sink->GetOwnerBlock().GetSampleTimes();

            for (size_t i = 0; i < originSampleTimes.size(); ++i) {
                if (originSampleTimes[i].GetSampleTimeType() != sinkSampleTimes[i].GetSampleTimeType()) {
                    throw std::runtime_error("Sample time mismatch detected between connected blocks.");
                }
            }
        }
    }


} // namespace PySysLinkBase
