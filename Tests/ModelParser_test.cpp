// Tests/test_model_parser.cpp

#include <gtest/gtest.h>
#include <PySysLinkBase/ModelParser.h>
#include <PySysLinkBase/IBlockFactory.h>
#include <PySysLinkBase/BlockEventsHandler.h>
#include <PySysLinkBase/SpdlogManager.h>
#include <map>
#include <memory>
#include <string>

#include "DummySimulationBlock.h"
#include "DummyBlockFactory.h"
#include "ModelParserTestFixture.h"



// Test that the simulation model is not null.
TEST_F(ModelParserTestFixture, SimulationModelNotNull) {
    ASSERT_NE(simulationModel, nullptr) << "The simulation model should not be null.";
}

// Test that simulation blocks are present.
TEST_F(ModelParserTestFixture, SimulationModelHasBlocks) {
    auto simulationBlocks = simulationModel->simulationBlocks;
    EXPECT_FALSE(simulationBlocks.empty()) << "The simulation model should contain at least one block.";
}

// Test that the simulation model has the correct number of blocks.
TEST_F(ModelParserTestFixture, SimulationModelHasCorrectNumberOfBlocks) {
    auto simulationBlocks = simulationModel->simulationBlocks;
    EXPECT_EQ(simulationBlocks.size(), 3) << "The simulation model should contain exactly 3 blocks.";
}

// Test that the simulation model has the correct number of links.
TEST_F(ModelParserTestFixture, SimulationModelHasCorrectNumberOfLinks) {
    auto portLinks = simulationModel->portLinks;
    EXPECT_EQ(portLinks.size(), 2) << "The simulation model should contain exactly 2 links.";
}

// Test that the simulation model contains the expected blocks.
TEST_F(ModelParserTestFixture, SimulationModelContainsExpectedBlocks) {
    auto simulationBlocks = simulationModel->simulationBlocks;
    EXPECT_EQ(simulationBlocks[0]->GetId(), "dummy1") << "The first block should be dummy1.";
    EXPECT_EQ(simulationBlocks[1]->GetId(), "dummy2") << "The second block should be dummy2.";
    EXPECT_EQ(simulationBlocks[2]->GetId(), "dummy3") << "The third block should be dummy3.";
}