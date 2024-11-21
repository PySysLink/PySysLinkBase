#ifndef SRC_PY_SYS_LINK_BASE_MODEL_PARSER
#define SRC_PY_SYS_LINK_BASE_MODEL_PARSER

#include "SimulationModel.h"
#include <string>
#include "ConfigurationValue.h"
#include <yaml-cpp/yaml.h>

namespace PySysLinkBase
{
    class ModelParser
    {
        private:
            static ConfigurationValue YamlToConfigurationValue(const YAML::Node& node);
            static std::vector<std::unique_ptr<PortLink>> ParseLinks(std::vector<std::map<std::string, ConfigurationValue>> linksConfigurations);
            static std::vector<std::unique_ptr<ISimulationBlock>> ParseBlocks(std::vector<std::map<std::string, ConfigurationValue>> blocksConfigurations);
        public:
            static SimulationModel ParseFromYaml(std::string filename);
    };
} // namespace PySysLinkBase


#endif /* SRC_PY_SYS_LINK_BASE_MODEL_PARSER */
