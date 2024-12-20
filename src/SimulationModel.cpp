#include "SimulationModel.h"
#include <stdexcept>
#include <unordered_set>
#include <numeric>
#include "spdlog/spdlog.h"

namespace PySysLinkBase
{  
    SimulationModel::SimulationModel(std::vector<std::shared_ptr<ISimulationBlock>> simulationBlocks, std::vector<std::shared_ptr<PortLink>> portLinks, std::shared_ptr<IBlockEventsHandler> blockEventsHandler) 
    {
        this->simulationBlocks.insert(this->simulationBlocks.end(), std::make_move_iterator(simulationBlocks.begin()), std::make_move_iterator(simulationBlocks.end()));
        this->portLinks.insert(this->portLinks.end(), std::make_move_iterator(portLinks.begin()), std::make_move_iterator(portLinks.end()));
        this->blockEventsHandler = blockEventsHandler;
    }

    const std::vector<std::shared_ptr<InputPort>> SimulationModel::GetConnectedPorts(const std::shared_ptr<ISimulationBlock> originBlock, int outputPortIndex) const
    {
        std::vector<std::shared_ptr<InputPort>> connectedPorts = {};
        for (int i=0; i < this->portLinks.size(); i++)
        {
            if (this->portLinks[i]->originBlock == originBlock && this->portLinks[i]->originBlockPortIndex == outputPortIndex)
            {
                connectedPorts.push_back(this->portLinks[i]->sinkBlock->GetInputPorts()[this->portLinks[i]->sinkBlockPortIndex]);
            }
        }
        return connectedPorts;
    }
    
    const std::pair<std::vector<std::shared_ptr<ISimulationBlock>>, std::vector<int>> SimulationModel::GetConnectedBlocks(const std::shared_ptr<ISimulationBlock> originBlock, int outputPortIndex) const
    {
        std::vector<std::shared_ptr<ISimulationBlock>> connectedBlocks = {};
        std::vector<int> connectedPortIndexes = {};
        for (int i=0; i < this->portLinks.size(); i++)
        {
            if (this->portLinks[i]->originBlock == originBlock && this->portLinks[i]->originBlockPortIndex == outputPortIndex)
            {
                connectedBlocks.push_back(this->portLinks[i]->sinkBlock);
                connectedPortIndexes.push_back(this->portLinks[i]->sinkBlockPortIndex);
            }
        }
        return std::pair<std::vector<std::shared_ptr<ISimulationBlock>>, std::vector<int>>(connectedBlocks, connectedPortIndexes);
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
            spdlog::get("default_pysyslink")->debug("Empty end reached");
            resultChains.push_back(currentChain);
            return;
        }

        // Iterate over all output ports
        for (int i = 0; i < outputPorts.size(); i++) 
        {
            // Get the input ports connected to this output
            const std::pair<std::vector<std::shared_ptr<ISimulationBlock>>, std::vector<int>> connectedBlocksInfoPair = this->GetConnectedBlocks(currentBlock, i);
            const std::vector<std::shared_ptr<ISimulationBlock>> connectedBlocks = connectedBlocksInfoPair.first;
            const std::vector<int> connectedPortIndexes = connectedBlocksInfoPair.second;
            if (connectedBlocks.empty()) 
            {
                // End the chain if the output port has no connections
                spdlog::get("default_pysyslink")->debug("No connection found, chain end");

                resultChains.push_back(currentChain);
                continue;
            }

            // For each connected input port
            for (int j = 0; j < connectedBlocks.size(); j++) 
            {
                if (connectedBlocks[j]->IsInputDirectBlockChainEnd(connectedPortIndexes[j])) 
                {
                    // End the chain if the connected input port stops direct feedthrough
                    currentChain.push_back(connectedBlocks[j]);
                    resultChains.push_back(currentChain);
                    spdlog::get("default_pysyslink")->debug("Direct block chain end reached");
                    continue;
                }

                // Otherwise, continue the chain with the owner block of the connected input port
                this->FindChains(connectedBlocks[j], currentChain, resultChains);
            }
        }
    }

    const std::vector<std::shared_ptr<ISimulationBlock>> SimulationModel::OrderBlockChainsOntoFreeOrder(const std::vector<std::vector<std::shared_ptr<ISimulationBlock>>> directBlockChains)
    {
        std::vector<std::shared_ptr<ISimulationBlock>> orderedBlocks;
        std::unordered_set<std::shared_ptr<ISimulationBlock>> processedBlocks;

        // A lambda to check if all feedthrough inputs of a block are processed
        auto areInputsResolved = [&](const std::shared_ptr<ISimulationBlock>& block) -> bool {
            for (int i = 0; i < block->GetInputPorts().size(); i++) {
                if (block->GetInputPorts()[i]->HasDirectFeedtrough()) {
                    auto originBlock = GetOriginBlock(block, i);
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

    const std::shared_ptr<ISimulationBlock> SimulationModel::GetOriginBlock(const std::shared_ptr<ISimulationBlock> sinkBlock, int sinkBlockPortIndex) const 
    {
        for (const auto& link : portLinks) {
            if (link->sinkBlock == sinkBlock && link->sinkBlockPortIndex == sinkBlockPortIndex) {
                return link->originBlock;
            }
        }
        return nullptr; // No connection found
    }

    void SimulationModel::PropagateSampleTimes() {
        // Helper lambdas
        auto hasKnownSampleTime = [](const std::shared_ptr<SampleTime> sampleTime) -> bool {
            return sampleTime->GetSampleTimeType() != SampleTimeType::inherited;
        };

        auto areSampleTimesCompatible = [](const SampleTime& st1, const SampleTime& st2) -> bool {
            if (st1.GetSampleTimeType() == SampleTimeType::constant && st2.GetSampleTimeType() == SampleTimeType::constant) {
                return true;
            }
            if (st1.GetSampleTimeType() == SampleTimeType::continuous && st2.GetSampleTimeType() == SampleTimeType::continuous) {
                return true;
            }
            if (st1.GetSampleTimeType() == SampleTimeType::discrete && st2.GetSampleTimeType() == SampleTimeType::discrete) {
                double gcdSampleTimeMs = std::gcd(static_cast<int>(st1.GetDiscreteSampleTime()*1000), static_cast<int>(st2.GetDiscreteSampleTime()*1000));
                return gcdSampleTimeMs > 0;
            }
            return false;
        };

        auto resolveSampleTime = [](const std::shared_ptr<SampleTime> st1, const std::shared_ptr<SampleTime> st2) -> std::shared_ptr<SampleTime> {
            if (st1->GetSampleTimeType() == SampleTimeType::constant && st2->GetSampleTimeType() == SampleTimeType::constant) {
                return st1; // Constants resolve to constant
            }
            if (st1->GetSampleTimeType() == SampleTimeType::continuous && st2->GetSampleTimeType() == SampleTimeType::continuous) {
                return st1; // Continuous resolves to continuous
            }
            if (st1->GetSampleTimeType() == SampleTimeType::discrete && st2->GetSampleTimeType() == SampleTimeType::discrete) {
                double gcdSampleTimeMs = std::gcd(static_cast<int>(st1->GetDiscreteSampleTime()*1000), static_cast<int>(st2->GetDiscreteSampleTime()*1000));
                if (gcdSampleTimeMs > 0) {
                    return std::make_shared<SampleTime>(SampleTimeType::discrete, gcdSampleTimeMs/1000);
                } else {
                    throw std::runtime_error("Discrete sample times are incompatible: No common multiple found.");
                }
            }
            if (st1->GetSampleTimeType() == SampleTimeType::constant) return st2;
            if (st2->GetSampleTimeType() == SampleTimeType::constant) return st1;

            throw std::runtime_error("Incompatible sample times: Continuous and discrete types cannot mix.");
        };

        spdlog::get("default_pysyslink")->debug("Forward sample time propagation start");
        // Forward propagation
        bool progressMade = true;
        while (progressMade) {
            progressMade = false;

            for (const auto& block : simulationBlocks) {
                bool allInputsResolved = true;
                std::vector<std::shared_ptr<SampleTime>> inputSampleTimes;

                for (int i = 0; i < block->GetInputPorts().size(); i++) {
                    const auto originBlock = GetOriginBlock(block, i);
                    if (!originBlock || !hasKnownSampleTime(originBlock->GetSampleTime())) {
                        allInputsResolved = false;
                        break;
                    }
                    inputSampleTimes.push_back(originBlock->GetSampleTime());
                }

                if (allInputsResolved) {
                    spdlog::get("default_pysyslink")->debug("All inputs resolved for block: {}", block->GetId());
                    const std::shared_ptr<SampleTime> blockSampleTime = block->GetSampleTime();
                    if (blockSampleTime->GetSampleTimeType() == SampleTimeType::inherited) {
                        spdlog::get("default_pysyslink")->debug("Inherited sample time");
                        std::shared_ptr<SampleTime> resolvedSampleTime = inputSampleTimes.front();
                        for (const auto& inputSampleTime : inputSampleTimes) {
                            resolvedSampleTime = resolveSampleTime(resolvedSampleTime, inputSampleTime);
                        }
                        block->SetSampleTime(resolvedSampleTime);
                        progressMade = true;
                    }
                }
            }
        }

        spdlog::get("default_pysyslink")->debug("Start backward propagation of sample time");

        // Backward propagation
        progressMade = true;
        while (progressMade) {
            progressMade = false;

            for (const auto& block : simulationBlocks) {
                bool allOutputsResolved = true;
                std::vector<std::shared_ptr<SampleTime>> outputSampleTimes;

                spdlog::get("default_pysyslink")->debug("Start working with block: {}", block->GetId());
                for (int i = 0; i < block->GetOutputPorts().size(); i++) {
                    const std::pair<std::vector<std::shared_ptr<ISimulationBlock>>, std::vector<int>> connectedBlocksInfoPair = this->GetConnectedBlocks(block, i);
                    const std::vector<std::shared_ptr<ISimulationBlock>> connectedBlocks = connectedBlocksInfoPair.first;
                    const std::vector<int> connectedPortIndexes = connectedBlocksInfoPair.second;
                    for (int j = 0; j < connectedBlocks.size(); j++) {
                        if (!connectedBlocks[j] || !hasKnownSampleTime(connectedBlocks[j]->GetSampleTime())) {
                            allOutputsResolved = false;
                            break;
                        }
                        outputSampleTimes.push_back(connectedBlocks[j]->GetSampleTime());
                    }
                }

                if (allOutputsResolved) {
                    spdlog::get("default_pysyslink")->debug("Start propagating with block: {}", block->GetId());

                    const std::shared_ptr<SampleTime> blockSampleTime = block->GetSampleTime();
                    spdlog::get("default_pysyslink")->debug("Sample time type: {}", SampleTime::SampleTimeTypeString(blockSampleTime->GetSampleTimeType()));
                    if (blockSampleTime->GetSampleTimeType() == SampleTimeType::inherited) {
                        std::shared_ptr<SampleTime> resolvedSampleTime = outputSampleTimes.front();
                        for (const auto& outputSampleTime : outputSampleTimes) {
                            resolvedSampleTime = resolveSampleTime(resolvedSampleTime, outputSampleTime);
                        }
                        block->GetSampleTime() = resolvedSampleTime;
                        progressMade = true;
                    }
                }
            }
        }
        spdlog::get("default_pysyslink")->debug("Validation start");

        // Final validation
        for (const auto& block : simulationBlocks) {
            if (!hasKnownSampleTime(block->GetSampleTime())) {
                throw std::runtime_error("Sample time propagation failed: Unresolved sample times remain.");
            }
        }
    }



} // namespace PySysLinkBase
