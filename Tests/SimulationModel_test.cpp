// Tests/test_model_parser.cpp

#include <gtest/gtest.h>
#include <PySysLinkBase/SimulationModel.h>
#include <PySysLinkBase/IBlockFactory.h>
#include <PySysLinkBase/BlockEventsHandler.h>
#include <PySysLinkBase/SpdlogManager.h>
#include <map>
#include <memory>
#include <string>

#include "DummySimulationBlock.h"
#include "DummyBlockFactory.h"
#include "ModelParserTestFixture.h"

// Test that GetConnectedPorts returns a non-empty result for a given block and output port index.
TEST_F(ModelParserTestFixture, GetConnectedPortsTest) {
    // Assume the dummy block "dummy1" (index 0) has a connection from its output.
    auto connectedPorts = simulationModel->GetConnectedPorts(simulationModel->simulationBlocks[0], 0);
    EXPECT_FALSE(connectedPorts.empty()) << "Connected ports for dummy1 should not be empty.";
}

TEST_F(ModelParserTestFixture, GetConnectedPortsTestFalse) {
    // Assume the dummy block "dummy1" (index 0) has a connection from its output.
    auto connectedPorts = simulationModel->GetConnectedPorts(simulationModel->simulationBlocks[2], 0);
    EXPECT_TRUE(connectedPorts.empty()) << "Connected ports for dummy3 should be empty.";
}

// Test that GetConnectedBlocks returns a pair with non-empty block list and matching port index list.
TEST_F(ModelParserTestFixture, GetConnectedBlocksTest) {
    auto result = simulationModel->GetConnectedBlocks(simulationModel->simulationBlocks[0], 0);
    const auto& connectedBlocks = std::get<0>(result);
    const auto& portIndices = std::get<1>(result);
    EXPECT_FALSE(connectedBlocks.empty()) << "Connected blocks for dummy1 should not be empty.";
    EXPECT_EQ(connectedBlocks.size(), portIndices.size()) << "Mismatch in the number of connected blocks and port indices.";
}

// Test that GetOriginBlock returns the expected origin block given a sink block and an input port index.
TEST_F(ModelParserTestFixture, GetOriginBlockTest) {
    // In our dummy configuration, we assume that dummy2's input (port index 0) originates from dummy1.
    auto originBlock = simulationModel->GetOriginBlock(simulationModel->simulationBlocks[1], 0);
    ASSERT_NE(originBlock, nullptr);
    EXPECT_EQ(originBlock->GetId(), "dummy1") << "The origin block for dummy2's input port 0 should be dummy1.";
}

// Test that GetDirectBlockChains returns at least one chain.
TEST_F(ModelParserTestFixture, GetDirectBlockChainsTest) {
    auto chains = simulationModel->GetDirectBlockChains();
    EXPECT_FALSE(chains.empty()) << "There should be at least one direct block chain.";
}

// Test that OrderBlockChainsOntoFreeOrder returns the expected number of blocks in the ordered chain.
TEST_F(ModelParserTestFixture, OrderBlockChainsOntoFreeOrderTest) {
    auto chains = simulationModel->GetDirectBlockChains();
    auto orderedBlocks = simulationModel->OrderBlockChainsOntoFreeOrder(chains);
    EXPECT_EQ(orderedBlocks.size(), simulationModel->simulationBlocks.size())
        << "Ordered blocks should match the number of simulation blocks.";
}

// Test that PropagateSampleTimes sets a non-null sample time for each block.
TEST_F(ModelParserTestFixture, PropagateSampleTimesTest) {
    simulationModel->PropagateSampleTimes();
    for (const auto& block : simulationModel->simulationBlocks) {
        EXPECT_NE(block->GetSampleTime(), nullptr) << "Each block should have a non-null sample time after propagation.";
    }
}