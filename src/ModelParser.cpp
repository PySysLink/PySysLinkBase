#include "ModelParser.h"
#include "ISimulationBlock.h"
#include "PortLink.h"
#include <unordered_map>
#include <variant>
#include "ConfigurationValue.h"
#include "spdlog/spdlog.h"
#include <regex>
#include <sstream>

namespace PySysLinkBase
{
    SimulationModel ModelParser::ParseFromYaml(std::string filename, const std::map<std::string, std::shared_ptr<IBlockFactory>>& blockFactories, std::shared_ptr<IBlockEventsHandler> blockEventsHandler)
    {
        YAML::Node config = YAML::LoadFile(filename);
       
        spdlog::get("default_pysyslink")->debug("File readed: {}", filename);

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

            spdlog::get("default_pysyslink")->debug("Configurations parsed");

            std::vector<std::shared_ptr<ISimulationBlock>> blocks = ModelParser::ParseBlocks(blocksConfigurations, blockFactories, blockEventsHandler);
            spdlog::get("default_pysyslink")->debug("Blocks parsed");
            std::vector<std::shared_ptr<PortLink>> links = ModelParser::ParseLinks(linksConfigurations, blocks);
            
            spdlog::get("default_pysyslink")->debug("Blocks and links parsed");

            return SimulationModel(std::move(blocks), std::move(links), blockEventsHandler);
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
                        try {
                            return ModelParser::ParseComplex(node.as<std::string>());
                        } catch (...) {
                            return node.as<std::string>();
                        }
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
            
            std::vector<std::complex<double>> complexElements;
            
            bool areComplex = true;
            for (const auto& subNode : node) {
                if (subNode.IsScalar()) {
                    try {
                        complexElements.push_back(ModelParser::ParseComplex(subNode.as<std::string>()));
                    } catch (...) {
                        areComplex = false;
                        break;
                    }
                }
            }

            if (areComplex) {
                return complexElements;
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
                                try {
                                    elements.push_back(ModelParser::ParseComplex(subNode.as<std::string>()));
                                } catch (...) {
                                    elements.push_back(subNode.as<std::string>());
                                }
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

    
    std::vector<std::shared_ptr<PortLink>> ModelParser::ParseLinks(std::vector<std::map<std::string, ConfigurationValue>> linksConfigurations, const std::vector<std::shared_ptr<ISimulationBlock>>& blocks)
    {
        std::vector<std::shared_ptr<PortLink>> links = {};
        for (int i = 0; i < linksConfigurations.size(); i++)
        {
            links.push_back(std::make_unique<PortLink>(PortLink::ParseFromConfig(linksConfigurations[i], blocks)));
        }
        return links;
    }

    std::vector<std::shared_ptr<ISimulationBlock>> ModelParser::ParseBlocks(std::vector<std::map<std::string, ConfigurationValue>> blocksConfigurations, const std::map<std::string, std::shared_ptr<IBlockFactory>>& blockFactories, std::shared_ptr<IBlockEventsHandler> blockEventsHandler)
    {
        std::vector<std::shared_ptr<ISimulationBlock>> blocks = {};
        for (int i = 0; i < blocksConfigurations.size(); i++)
        {
            std::string blockType = ConfigurationValueManager::TryGetConfigurationValue<std::string>("BlockType", blocksConfigurations[i]);
            spdlog::get("default_pysyslink")->debug("{} being configured...", blockType);   
            auto it = blockFactories.find(blockType);
            if (it == blockFactories.end()) {
                throw std::invalid_argument("There is no IBlockFactory for block type: " + blockType + ". Is it supported?");
            } else {
                blocks.push_back(std::move(it->second->CreateBlock(blocksConfigurations[i], blockEventsHandler)));
            }
        }
        return blocks;
    }

    std::complex<double> ModelParser::ParseComplex(const std::string& str) {
         std::regex complex_pattern(R"(\s*([-+]?\d*\.?\d+)?\s*([+-]\s*\d*\.?\d+)?\s*(i|j)?\s*)");

        std::smatch matches;
        if (std::regex_match(str, matches, complex_pattern)) {
            double real_part = 0.0;
            double imag_part = 0.0;

            // Parse the real part if it exists
            if (matches[1].matched) {
                real_part = std::stod(matches[1].str());
            }

            // Parse the imaginary part if it exists
            if (matches[2].matched) {
                // Remove any extra spaces in the imaginary part
                std::string imag_str = matches[2].str();
                imag_str.erase(remove(imag_str.begin(), imag_str.end(), ' '), imag_str.end());
                imag_part = std::stod(imag_str);
            }

            // If no imaginary part is provided but 'i' or 'j' exists, treat it as 1 or -1
            if (matches[2].str().empty() && (matches[3].str() == "i" || matches[3].str() == "j")) {
                imag_part = matches[1].matched ? 1.0 : -1.0;
            }

            return std::complex<double>(real_part, imag_part);
        } else {
            throw std::invalid_argument("Invalid complex number format: " + str);
        }
    }
} // namespace PySysLinkBase
