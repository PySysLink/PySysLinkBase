// DummyBlockFactory.h
#ifndef DUMMY_BLOCK_FACTORY_H
#define DUMMY_BLOCK_FACTORY_H

#include "PySysLinkBase/IBlockFactory.h"
#include "DummySimulationBlock.h"
#include <memory>
#include <map>
#include <string>
#include <stdexcept>

class DummyBlockFactory : public PySysLinkBase::IBlockFactory {
public:
    std::shared_ptr<PySysLinkBase::ISimulationBlock> CreateBlock(
        std::map<std::string, PySysLinkBase::ConfigurationValue> blockConfiguration,
        std::shared_ptr<PySysLinkBase::IBlockEventsHandler> blockEventsHandler) override 
    {
        std::string blockClass = PySysLinkBase::ConfigurationValueManager::TryGetConfigurationValue<std::string>("BlockClass", blockConfiguration);
        if (blockClass == "dummy/Initial")
        {
            return std::make_shared<DummySimulationBlock>(blockConfiguration, blockEventsHandler, 0, 1);
        }
        else if (blockClass == "dummy/Final")
        {
            return std::make_shared<DummySimulationBlock>(blockConfiguration, blockEventsHandler, 1, 0);
        }
        else if (blockClass == "dummy/Intermediate")
        {
            return std::make_shared<DummySimulationBlock>(blockConfiguration, blockEventsHandler, 1, 1);
        }
        else
        {
            throw std::out_of_range("Block type with id: " + blockClass + " not found in module Dummy.");
        }
    }
};

#endif /* DUMMY_BLOCK_FACTORY_H */
