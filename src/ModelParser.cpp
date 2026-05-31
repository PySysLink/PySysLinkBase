#include "ModelParser.h"
#include "ISimulationBlock.h"
#include "PortLink.h"

#include <unordered_map>
#include <variant>
#include <regex>
#include <sstream>

#include "ConfigurationValue.h"
#include "spdlog/spdlog.h"


namespace PySysLinkBase
{
    ParsedConfigurationKey ModelParser::ParseConfigurationKey(
        const std::string& rawKey)
    {
        auto openBracket = rawKey.find('[');
        auto closeBracket = rawKey.find(']');

        if (openBracket == std::string::npos ||
            closeBracket == std::string::npos ||
            closeBracket <= openBracket)
        {
            throw std::runtime_error(
                "Configuration key does not contain explicit type information: "
                + rawKey
            );
        }

        ParsedConfigurationKey parsedKey;

        parsedKey.keyName =
            rawKey.substr(0, openBracket);

        parsedKey.typeName =
            rawKey.substr(
                openBracket + 1,
                closeBracket - openBracket - 1
            );

        return parsedKey;
    }
    
    std::shared_ptr<SimulationModel> ModelParser::ParseFromYaml(std::string filename, const std::map<std::string, std::shared_ptr<IBlockFactory>>& blockFactories, std::shared_ptr<IBlockEventsHandler> blockEventsHandler)
    {
        YAML::Node config;
        try
        {
            config = YAML::LoadFile(filename);
        }
        catch (YAML::BadFile& e)
        {
            throw std::runtime_error("Could not read file: " + filename);
        }
       
        spdlog::get("default_pysyslink")->debug("File read: {}", filename);

        if (!(config["Blocks"] && config["Links"]))
        {
            throw std::runtime_error("No 'Blocks' or 'Links' node found in YAML configuration.");
        }

        std::vector<std::map<std::string, ConfigurationValue>> blocksConfigurations = {};
        for (std::size_t i=0;i<config["Blocks"].size();i++) 
        {
            YAML::Node blockConfigurationYaml = config["Blocks"][i];
            std::map<std::string, ConfigurationValue> blockConfiguration = {};

            for(YAML::const_iterator it=blockConfigurationYaml.begin(); it!=blockConfigurationYaml.end(); ++it) {
                std::string rawKey = it->first.as<std::string>();

                ParsedConfigurationKey parsedKey = ParseConfigurationKey(rawKey);

                spdlog::get("default_pysyslink")->debug("Parsing block configuration key: {} of type {}", parsedKey.keyName, parsedKey.typeName);
                blockConfiguration.insert({parsedKey.keyName, ModelParser::YamlToConfigurationValue(it->second, parsedKey.typeName)});
            }
            blocksConfigurations.push_back(blockConfiguration);
        }
        std::vector<std::map<std::string, ConfigurationValue>> linksConfigurations = {};
        for (std::size_t i=0;i<config["Links"].size();i++) 
        {
            YAML::Node linkConfigurationYaml = config["Links"][i];
            std::map<std::string, ConfigurationValue> linkConfiguration = {};

            for(YAML::const_iterator it=linkConfigurationYaml.begin(); it!=linkConfigurationYaml.end(); ++it) {
                std::string rawKey = it->first.as<std::string>();

                ParsedConfigurationKey parsedKey = ParseConfigurationKey(rawKey);

                spdlog::get("default_pysyslink")->debug("Parsing link configuration key: {} of type {}", parsedKey.keyName, parsedKey.typeName);
                linkConfiguration.insert({parsedKey.keyName, ModelParser::YamlToConfigurationValue(it->second, parsedKey.typeName)});
            }
            linksConfigurations.push_back(linkConfiguration);
        }

        spdlog::get("default_pysyslink")->debug("Configurations parsed");

        std::vector<std::shared_ptr<ISimulationBlock>> blocks = ModelParser::ParseBlocks(blocksConfigurations, blockFactories, blockEventsHandler);
        spdlog::get("default_pysyslink")->debug("Blocks parsed");
        std::vector<std::shared_ptr<PortLink>> links = ModelParser::ParseLinks(linksConfigurations, blocks);
        
        spdlog::get("default_pysyslink")->debug("Blocks and links parsed");

        return std::make_shared<SimulationModel>(std::move(blocks), std::move(links), blockEventsHandler);
    }

    ConfigurationValue ModelParser::YamlToConfigurationValue(const YAML::Node& node, const std::string& typeName)
    {
        //
        // Scalar types
        //

        if (typeName == "int")
        {
            return node.as<int>();
        }

        if (typeName == "double")
        {
            return node.as<double>();
        }

        if (typeName == "bool")
        {
            return node.as<bool>();
        }

        if (typeName == "complex_double")
        {
            return ModelParser::ParseComplex(node.as<std::string>());
        }

        if (typeName == "string")
        {
            return node.as<std::string>();
        }

        //
        // Vector types
        //

        if (typeName == "vector<int>")
        {
            std::vector<int> values;

            for (const auto& subNode : node)
            {
                values.push_back(subNode.as<int>());
            }

            return values;
        }

        if (typeName == "vector<double>")
        {
            std::vector<double> values;

            for (const auto& subNode : node)
            {
                values.push_back(subNode.as<double>());
            }

            return values;
        }

        if (typeName == "vector<bool>")
        {
            std::vector<bool> values;

            for (const auto& subNode : node)
            {
                values.push_back(subNode.as<bool>());
            }

            return values;
        }

        if (typeName == "vector<complex_double>")
        {
            std::vector<std::complex<double>> values;

            for (const auto& subNode : node)
            {
                values.push_back(
                    ModelParser::ParseComplex(
                        subNode.as<std::string>()
                    )
                );
            }

            return values;
        }

        if (typeName == "vector<string>")
        {
            std::vector<std::string> values;

            for (const auto& subNode : node)
            {
                values.push_back(subNode.as<std::string>());
            }

            return values;
        }

        throw std::runtime_error(
            "Unsupported explicit configuration type: " + typeName
        );
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
