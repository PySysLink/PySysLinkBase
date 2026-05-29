#ifndef SRC_CONFIGURATION_VALUE
#define SRC_CONFIGURATION_VALUE

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include <complex>
#include "PortsAndSignalValues/PortTypeMetadata.h"
#include <numeric>

namespace PySysLinkBase
{    
    using ConfigurationValue = std::variant<
        int,
        double,
        bool,
        std::complex<double>,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<bool>,
        std::vector<std::complex<double>>,
        std::vector<std::string>
    >;

    class ConfigurationValueManager
    {
        public:
        template <typename T> 
        static T TryGetConfigurationValue(std::string keyName, std::map<std::string, ConfigurationValue> configurationValues)
        {
            ConfigurationValue foundValue;
            auto it = configurationValues.find(keyName);
            if (it == configurationValues.end()) {
                throw std::out_of_range("Key name: " + keyName + " not found in configuration, available keys are: " + std::accumulate(configurationValues.begin(), configurationValues.end(), std::string(""), [](std::string acc, std::pair<std::string, ConfigurationValue> pair) {
                    return acc + pair.first + ", ";
                }));
            } else {
                foundValue = it->second;
            }
            try
            {
                return std::get<T>(foundValue);
            }
            catch (std::bad_variant_access const& ex)
            {
                throw std::invalid_argument("Configuration option of key: " + keyName + " was not of expected type: " + ex.what());
            }
        }
    };
} // namespace PySysLinkBase



#endif /* SRC_CONFIGURATION_VALUE */
