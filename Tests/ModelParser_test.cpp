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
TEST(ModelParserMatrixParsing, ParsesMatlabStyleMatrixStrings) {
    YAML::Node node = YAML::Load("\"[1,2;3,4]\"");
    auto parsed = PySysLinkBase::ModelParser::YamlToConfigurationValue(node, "matrix<int>");

    const auto& matrix = std::get<PySysLinkBase::IntMatrix>(parsed);
    ASSERT_EQ(matrix.rows(), 2);
    ASSERT_EQ(matrix.cols(), 2);
    EXPECT_EQ(matrix(0, 0), 1);
    EXPECT_EQ(matrix(0, 1), 2);
    EXPECT_EQ(matrix(1, 0), 3);
    EXPECT_EQ(matrix(1, 1), 4);
}

TEST(ModelParserMatrixParsing, ParsesComplexMatlabStyleMatrixStrings) {
    YAML::Node node = YAML::Load("\"[1+2i, 3; 4-5j, 6]\"");
    auto parsed = PySysLinkBase::ModelParser::YamlToConfigurationValue(node, "matrix<complex_double>");

    const auto& matrix = std::get<PySysLinkBase::ComplexMatrix>(parsed);
    ASSERT_EQ(matrix.rows(), 2);
    ASSERT_EQ(matrix.cols(), 2);
    EXPECT_EQ(matrix(0, 0), std::complex<double>(1.0, 2.0));
    EXPECT_EQ(matrix(0, 1), std::complex<double>(3.0, 0.0));
    EXPECT_EQ(matrix(1, 0), std::complex<double>(4.0, -5.0));
    EXPECT_EQ(matrix(1, 1), std::complex<double>(6.0, 0.0));
}

TEST(ModelParserMatrixParsing, ParsesVectorOfMatlabStyleMatrixStrings) {
    YAML::Node node = YAML::Load("- \"[1 2; 3 4]\"\n- \"[5 6; 7 8]\"");
    auto parsed = PySysLinkBase::ModelParser::YamlToConfigurationValue(node, "vector<matrix<int>>");

    const auto& matrices = std::get<std::vector<PySysLinkBase::IntMatrix>>(parsed);
    ASSERT_EQ(matrices.size(), 2);
    EXPECT_EQ(matrices[0](1, 1), 4);
    EXPECT_EQ(matrices[1](0, 1), 6);
}