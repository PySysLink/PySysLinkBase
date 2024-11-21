#include "ModelParser.h"
#include <yaml-cpp/yaml.h>
#include "ISimulationBlock.h"
#include "PortLink.h"
#include <unordered_map>
#include <variant>
#include "ConfigurationValue.h"


namespace PySysLinkBase
{
    SimulationModel ModelParser::ParseFromYaml(std::string filename)
    {
        YAML::Node config = YAML::LoadFile("system1.yaml");

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

            std::vector<std::unique_ptr<ISimulationBlock>> blocks = ModelParser::ParseBlocks(blocksConfigurations);
            std::vector<std::unique_ptr<PortLink>> links = ModelParser::ParseLinks(linksConfigurations);

            return SimulationModel(std::move(blocks), std::move(links));
        } 
        else
        {
            throw std::runtime_error("Unsupported YAML node type.");
        }   
    }

    ConfigurationValue YamlToConfigurationValue(const YAML::Node& node) {

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

    std::vector<std::unique_ptr<PortLink>> ModelParser::ParseLinks(std::vector<std::map<std::string, ConfigurationValue>> linksConfigurations)
    {
        std::vector<std::unique_ptr<PortLink>> links = {};
        return links;
    }

    std::vector<std::unique_ptr<ISimulationBlock>> ModelParser::ParseBlocks(std::vector<std::map<std::string, ConfigurationValue>> blocksConfigurations)
    {
        std::vector<std::unique_ptr<ISimulationBlock>> blocks = {};
        return blocks;
    }
} // namespace PySysLinkBase
