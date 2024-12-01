#include "ModelParser.h"
#include "ISimulationBlock.h"
#include "PortLink.h"
#include <unordered_map>
#include <variant>
#include "ConfigurationValue.h"
#include <iostream>

namespace PySysLinkBase
{
    SimulationModel ModelParser::ParseFromYaml(std::string filename, const std::map<std::string, std::unique_ptr<IBlockFactory>>& blockFactories)
    {
        YAML::Node config = YAML::LoadFile(filename);
       
        std::cout << "File readed" << std::endl;

        if (config["Blocks"] && config["Links"]) {
            std::vector<std::map<std::string, ConfigurationValue>> blocksConfigurations = {};
            for (std::size_t i=0;i<config["Blocks"].size();i++) 
            {
                YAML::Node blockConfigurationYaml = config["Blocks"][i];
                std::map<std::string, ConfigurationValue> blockConfiguration = {};
                for(YAML::const_iterator it=blockConfigurationYaml.begin();it!=blockConfigurationYaml.end();++it) {
                    blockConfiguration.insert({it->first.as<std::string>(), ModelParser::YamlToConfigurationValue(it->second)});
                }
                blocksConfigurations.push_back(blockConfiguration);
            }
            std::vector<std::map<std::string, ConfigurationValue>> linksConfigurations = {};
            for (std::size_t i=0;i<config["Links"].size();i++) 
            {
                YAML::Node linkConfigurationYaml = config["Links"][i];
                std::map<std::string, ConfigurationValue> linkConfiguration = {};
                for(YAML::const_iterator it=linkConfigurationYaml.begin();it!=linkConfigurationYaml.end();++it) {
                    linkConfiguration.insert({it->first.as<std::string>(), ModelParser::YamlToConfigurationValue(it->second)});
                }
                linksConfigurations.push_back(linkConfiguration);
            }

            std::cout << "Configurations parsed" << std::endl;

            std::vector<std::unique_ptr<ISimulationBlock>> blocks = ModelParser::ParseBlocks(blocksConfigurations, blockFactories);
            std::cout << "Blocks parsed" << std::endl;
            std::vector<std::unique_ptr<PortLink>> links = ModelParser::ParseLinks(linksConfigurations, blocks);
            
            std::cout << "Blocks and links parsed" << std::endl;

            return SimulationModel(std::move(blocks), std::move(links));
        } 
        else
        {
            throw std::runtime_error("Unsupported YAML node type.");
        }   
    }

    ConfigurationValue ModelParser::YamlToConfigurationValue(const YAML::Node& node) {

        if (node.IsScalar()) {
            try {
                return node.as<int>();
            } catch (...) {
                try {
                    return node.as<double>();
                } catch (...) {
                    try {
                        return node.as<bool>();
                    } catch (...) {
                        return node.as<std::string>();
                    }
                }
            }
        } else if (node.IsSequence()) {
            std::vector<int> intElements;
            bool areInts = true;
            for (const auto& subNode : node) {
                if (subNode.IsScalar()) {
                    try {
                        intElements.push_back(subNode.as<int>());
                    } catch (...) {
                        areInts = false;
                        break;  
                    }
                }
            }
            if (areInts)
            {
                return intElements;
            }

            std::vector<double> doubleElements;
            bool areDoubles = true;
            for (const auto& subNode : node) {
                if (subNode.IsScalar()) {
                    try {
                        doubleElements.push_back(subNode.as<double>());
                    } catch (...) {
                        areDoubles = false;
                        break;
                    }
                }
            }

            if (areDoubles) {
                return doubleElements;
            }

            std::vector<bool> boolElements;
            
            bool areBool = true;
            for (const auto& subNode : node) {
                if (subNode.IsScalar()) {
                    try {
                        boolElements.push_back(subNode.as<bool>());
                    } catch (...) {
                        areBool = false;
                        break;
                    }
                }
            }

            if (areBool) {
                return boolElements;
            }

            std::vector<std::string> stringElements;
            bool areString = true;
            for (const auto& subNode : node) {
                if (subNode.IsScalar()) {
                    try {
                        stringElements.push_back(subNode.as<std::string>());
                    } catch (...) {
                        areString = false;
                        break;
                    }
                }
            }

            if (areString) {
                return stringElements;
            }

            std::vector<ConfigurationValuePrimitive> elements;
            for (const auto& subNode : node) {
                if (subNode.IsScalar()) {
                    try {
                        elements.push_back(subNode.as<int>());
                    } catch (...) {
                        try {
                            elements.push_back(subNode.as<double>());
                        } catch (...) {
                            try {
                                elements.push_back(subNode.as<bool>());
                            } catch (...) {
                                elements.push_back(subNode.as<std::string>());
                            }
                        }
                    }
                }
            }
            return elements;
        } else {
            throw std::runtime_error("Unsupported YAML node type.");
        }
    }

    
    std::vector<std::unique_ptr<PortLink>> ModelParser::ParseLinks(std::vector<std::map<std::string, ConfigurationValue>> linksConfigurations, const std::vector<std::unique_ptr<ISimulationBlock>>& blocks)
    {
        std::vector<std::unique_ptr<PortLink>> links = {};
        for (int i = 0; i < linksConfigurations.size(); i++)
        {
            links.push_back(std::make_unique<PortLink>(PortLink::ParseFromConfig(linksConfigurations[i], blocks)));
        }
        return links;
    }

    std::vector<std::unique_ptr<ISimulationBlock>> ModelParser::ParseBlocks(std::vector<std::map<std::string, ConfigurationValue>> blocksConfigurations, const std::map<std::string, std::unique_ptr<IBlockFactory>>& blockFactories)
    {
        std::vector<std::unique_ptr<ISimulationBlock>> blocks = {};
        for (int i = 0; i < blocksConfigurations.size(); i++)
        {
            std::string blockType = ConfigurationValueManager::TryGetConfigurationValue<std::string>("BlockType", blocksConfigurations[i]);
            std::cout << blockType << " being configured..." << std::endl;   
            auto it = blockFactories.find(blockType);
            if (it == blockFactories.end()) {
                throw std::invalid_argument("There is no IBlockFactory for block type: " + blockType + ". Is it supported?");
            } else {
                blocks.push_back(std::move(it->second->CreateBlock(blocksConfigurations[i])));
            }
        }
        return blocks;
    }
} // namespace PySysLinkBase
