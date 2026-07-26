#include "ModelParser.h"
#include "ISimulationBlock.h"
#include "PortLink.h"

#include <unordered_map>
#include <variant>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <type_traits>

#include "ConfigurationValue.h"
#include "spdlog/spdlog.h"


namespace PySysLinkBase
{
    template<typename T>
    std::vector<T> ParseVector(const YAML::Node& node);

    template<typename T>
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> ParseMatrix(const YAML::Node& node);

    template<typename T>
    std::vector<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> ParseMatrixVector(const YAML::Node& node);

    ComplexMatrix ParseComplexMatrix(const YAML::Node& node);
    std::vector<ComplexMatrix> ParseComplexMatrixVector(const YAML::Node& node);

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
            return ParseVector<int>(node);

        if (typeName == "vector<double>")
            return ParseVector<double>(node);

        if (typeName == "vector<bool>")
            return ParseVector<bool>(node);

        if (typeName == "vector<string>")
            return ParseVector<std::string>(node);

        if (typeName == "vector<complex_double>")
            return ParseVector<std::complex<double>>(node);

        if (typeName == "matrix<int>")
            return ParseMatrix<int>(node);

        if (typeName == "matrix<double>")
            return ParseMatrix<double>(node);

        if (typeName == "matrix<bool>")
            return ParseMatrix<bool>(node);

        if (typeName == "matrix<complex_double>")
            return ParseComplexMatrix(node);

        if (typeName == "vector<matrix<int>>")
            return ParseMatrixVector<int>(node);

        if (typeName == "vector<matrix<double>>")
            return ParseMatrixVector<double>(node);

        if (typeName == "vector<matrix<bool>>")
            return ParseMatrixVector<bool>(node);

        if (typeName == "vector<matrix<complex_double>>")
            return ParseComplexMatrixVector(node);

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

    template<typename T>
    std::vector<T> ParseVector(const YAML::Node& node)
    {
        std::vector<T> values;
        values.reserve(node.size());

        for (const auto& subNode : node)
        {
            if constexpr (std::is_same_v<T, std::complex<double>>)
                values.push_back(ModelParser::ParseComplex(subNode.as<std::string>()));
            else
                values.push_back(subNode.as<T>());
        }

        return values;
    }

    std::string Trim(const std::string& input)
    {
        const auto begin = std::find_if(input.begin(), input.end(), [](unsigned char ch){ return !std::isspace(ch); });
        if (begin == input.end())
            return {};

        const auto end = std::find_if(input.rbegin(), input.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base();
        return std::string(begin, end);
    }

    std::complex<double> ModelParser::ParseComplex(const std::string& str)
    {
        std::string text = Trim(str);
        spdlog::get("default_pysyslink")->debug("ParseComplex input='{}'", text);

        if (text.empty())
            throw std::invalid_argument("Invalid complex number format: " + str);

        while (!text.empty() && (text.front() == '(' || text.front() == '['))
            text.erase(text.begin());
        while (!text.empty() && (text.back() == ')' || text.back() == ']'))
            text.pop_back();
        text = Trim(text);

        bool imagUnit = false;
        if (!text.empty() && (text.back() == 'i' || text.back() == 'j'))
        {
            imagUnit = true;
            text.pop_back();
            text = Trim(text);
        }

        if (text.empty() || text == "+" || text == "-")
            return std::complex<double>(0.0, text == "-" ? -1.0 : 1.0);

        std::size_t splitPos = std::string::npos;
        for (std::size_t i = 1; i < text.size(); ++i)
        {
            const char c = text[i];
            if ((c == '+' || c == '-') && text[i - 1] != 'e' && text[i - 1] != 'E')
            {
                splitPos = i;
                break;
            }
        }

        try
        {
            if (splitPos != std::string::npos)
            {
                const std::string realPart = Trim(text.substr(0, splitPos));
                const std::string imagPart = Trim(text.substr(splitPos));
                spdlog::get("default_pysyslink")->debug("ParseComplex split real='{}' imag='{}'", realPart, imagPart);
                const double real = realPart.empty() ? 0.0 : std::stod(realPart);
                const double imag = imagPart.empty() ? 1.0 : std::stod(imagPart);
                return std::complex<double>(real, imag);
            }

            const double value = std::stod(text);
            return imagUnit ? std::complex<double>(0.0, value) : std::complex<double>(value, 0.0);
        }
        catch (const std::exception& ex)
        {
            spdlog::get("default_pysyslink")->error("ParseComplex failed for token '{}' : {}", text, ex.what());
            throw;
        }
    }

    namespace
    {
        template<typename T>
        T ParseScalarToken(const std::string& token)
        {
            if constexpr (std::is_same_v<T, bool>)
            {
                std::string normalized = token;
                std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch){ return static_cast<char>(std::tolower(ch)); });
                if (normalized == "true" || normalized == "1")
                    return true;
                if (normalized == "false" || normalized == "0")
                    return false;
                throw std::invalid_argument("Invalid boolean token: " + token);
            }
            else if constexpr (std::is_same_v<T, std::complex<double>>)
            {
                return ModelParser::ParseComplex(token);
            }
            else
            {
                return static_cast<T>(std::stod(token));
            }
        }

        template<typename T>
        T ParseYamlScalar(const YAML::Node& node)
        {
            if constexpr (std::is_same_v<T, std::complex<double>>)
            {
                return ModelParser::ParseComplex(node.as<std::string>());
            }
            else
            {
                return node.as<T>();
            }
        }

        template<typename T>
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
        ParsePythonMatrixString(const std::string& text)
        {
            const std::string trimmed = Trim(text);
            if (trimmed.size() < 2 || trimmed.front() != '[' || trimmed.back() != ']')
                throw std::runtime_error("Matrix string must be enclosed in square brackets.");

            std::vector<std::vector<T>> rows;
            std::size_t i = 0;
            const std::size_t n = trimmed.size();

            auto skipWhitespace = [&]() {
                while (i < n && std::isspace(static_cast<unsigned char>(trimmed[i])))
                    ++i;
            };

            skipWhitespace();
            if (i >= n || trimmed[i] != '[')
                throw std::runtime_error("Expected '['.");
            ++i;
            skipWhitespace();

            while (i < n)
            {
                if (trimmed[i] == ']')
                {
                    ++i;
                    break;
                }

                if (trimmed[i] != '[')
                    throw std::runtime_error("Expected row '['.");
                ++i;

                std::vector<T> row;
                while (true)
                {
                    skipWhitespace();
                    if (i >= n)
                        throw std::runtime_error("Unexpected end of matrix.");

                    std::string token;
                    int parenDepth = 0;
                    while (i < n)
                    {
                        char c = trimmed[i];
                        if (c == '(')
                            ++parenDepth;
                        else if (c == ')')
                            --parenDepth;

                        if (parenDepth == 0 && (c == ',' || c == ']'))
                            break;

                        token += c;
                        ++i;
                    }

                    token = Trim(token);
                    if (token.empty())
                        throw std::runtime_error("Empty matrix element.");

                    spdlog::get("default_pysyslink")->debug("ParsePythonMatrixString token='{}'", token);
                    row.push_back(ParseScalarToken<T>(token));

                    skipWhitespace();
                    if (i >= n)
                        throw std::runtime_error("Unexpected end of matrix.");

                    if (trimmed[i] == ',')
                    {
                        ++i;
                        continue;
                    }

                    if (trimmed[i] == ']')
                    {
                        ++i;
                        break;
                    }

                    throw std::runtime_error("Expected ',' or ']'.");
                }

                rows.push_back(std::move(row));
                skipWhitespace();

                if (i >= n)
                    break;

                if (trimmed[i] == ',')
                {
                    ++i;
                    skipWhitespace();
                    continue;
                }

                if (trimmed[i] == ']')
                {
                    ++i;
                    break;
                }

                throw std::runtime_error("Expected ',' or ']'.");
            }

            if (rows.empty())
                throw std::runtime_error("Matrix cannot be empty.");

            const std::size_t cols = rows.front().size();
            for (const auto& row : rows)
            {
                if (row.size() != cols)
                    throw std::runtime_error("Matrix rows have different lengths.");
            }

            Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> mat(rows.size(), cols);
            for (std::size_t r = 0; r < rows.size(); ++r)
                for (std::size_t c = 0; c < cols; ++c)
                    mat(r, c) = rows[r][c];

            return mat;
        }
    }

    template<typename T>
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> ParseMatrix(const YAML::Node& node)
    {
        if (node.IsScalar())
            return ParsePythonMatrixString<T>(node.as<std::string>());

        if (!node.IsSequence() || node.size() == 0)
            throw std::runtime_error("Matrix must be a non-empty sequence.");

        if (!node[0].IsSequence())
        {
            Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> mat(1, node.size());
            for (std::size_t c = 0; c < node.size(); ++c)
                mat(0, c) = ParseYamlScalar<T>(node[c]);
            return mat;
        }

        const auto rows = node.size();
        const auto cols = node[0].size();

        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> mat(rows, cols);

        for (std::size_t r = 0; r < rows; ++r)
        {
            if (!node[r].IsSequence() || node[r].size() != cols)
                throw std::runtime_error("Matrix rows have different lengths.");

            for (std::size_t c = 0; c < cols; ++c)
                mat(r, c) = ParseYamlScalar<T>(node[r][c]);
        }

        return mat;
    }

    ComplexMatrix ParseComplexMatrix(const YAML::Node& node)
    {
        return ParseMatrix<std::complex<double>>(node);
    }

    template<typename T>
    std::vector<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>
    ParseMatrixVector(const YAML::Node& node)
    {
        std::vector<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>> values;
        values.reserve(node.size());

        for (const auto& subNode : node)
        {
            if (subNode.IsScalar())
                values.push_back(ParsePythonMatrixString<T>(subNode.as<std::string>()));
            else
                values.push_back(ParseMatrix<T>(subNode));
        }

        return values;
    }

    std::vector<ComplexMatrix> ParseComplexMatrixVector(const YAML::Node& node)
    {
        std::vector<ComplexMatrix> values;
        values.reserve(node.size());

        for (const auto& subNode : node)
        {
            if (subNode.IsScalar())
                values.push_back(ParsePythonMatrixString<std::complex<double>>(subNode.as<std::string>()));
            else
                values.push_back(ParseComplexMatrix(subNode));
        }

        return values;
    }
} // namespace PySysLinkBase
