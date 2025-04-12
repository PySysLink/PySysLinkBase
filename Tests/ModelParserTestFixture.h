// Tests/test_model_parser_fixture.h
#ifndef MODEL_PARSER_TEST_FIXTURE_H
#define MODEL_PARSER_TEST_FIXTURE_H

#include <gtest/gtest.h>
#include <PySysLinkBase/ModelParser.h>
#include <PySysLinkBase/IBlockFactory.h>
#include <PySysLinkBase/BlockEventsHandler.h>
#include <PySysLinkBase/SpdlogManager.h>
#include <map>
#include <memory>
#include <string>

// Include your dummy implementations
#include "DummySimulationBlock.h"
#include "DummyBlockFactory.h"

// Define a fixture class
class ModelParserTestFixture : public ::testing::Test {
protected:
    // Shared objects to be used in tests
    std::shared_ptr<PySysLinkBase::SimulationModel> simulationModel;
    std::shared_ptr<PySysLinkBase::BlockEventsHandler> blockEventsHandler;
    std::map<std::string, std::shared_ptr<PySysLinkBase::IBlockFactory>> blockFactories;
    std::string yamlFile;

    // This function is called before each test case.
    void SetUp() override {
        // Configure logging
        try
        {
            PySysLinkBase::SpdlogManager::ConfigureDefaultLogger();
            PySysLinkBase::SpdlogManager::SetLogLevel(PySysLinkBase::LogLevel::off);
        }
        catch(const std::exception& e)
        {
            ;
        }
        
        

        // Create a block events handler
        blockEventsHandler = std::make_shared<PySysLinkBase::BlockEventsHandler>();

        // Create a dummy block factory and add it to the registry.
        blockFactories["dummy"] = std::make_shared<DummyBlockFactory>();

        // Set the YAML file path (adjust the path if needed)
        yamlFile = "../Tests/dummy_system.yaml";

        // Parse the YAML file to create a simulation model.
        simulationModel = PySysLinkBase::ModelParser::ParseFromYaml(yamlFile, blockFactories, blockEventsHandler);
    }

    // Optionally, define TearDown() if you need to clean up after each test.
    void TearDown() override {
        // Cleanup code (if needed)
    }
};

#endif // MODEL_PARSER_TEST_FIXTURE_H
