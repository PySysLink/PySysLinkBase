#ifndef SRC_PY_SYS_LINK_BASE_CONFIGURATION_VALUE
#define SRC_PY_SYS_LINK_BASE_CONFIGURATION_VALUE

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>

namespace PySysLinkBase
{
    using ConfigurationValuePrimitive  = std::variant<
        int,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<bool>,
        std::vector<std::string>>;
    
    using ConfigurationValue = std::variant<
        int,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<bool>,
        std::vector<std::string>,
        ConfigurationValuePrimitive,
        std::vector<ConfigurationValuePrimitive>>;

    class ConfigurationValueManager
    {
        public:
        template <typename T> 
        static T TryGetConfigurationValue(std::string keyName, std::map<std::string, ConfigurationValue> configurationValues)
        {
            ConfigurationValue findedValue;
            auto it = configurationValues.find(keyName);
            if (it == configurationValues.end()) {
                throw std::out_of_range("Key name: " + keyName + " not found in configuration.");
            } else {
                findedValue = it->second;
            }
            try
            {
                return std::get<T>(findedValue);
            }
            catch (std::bad_variant_access const& ex)
            {
                throw std::invalid_argument("Configuration option of key: " + keyName + " was not of expected type: " + ex.what());
            }
        }
    };
} // namespace PySysLinkBase



#endif /* SRC_PY_SYS_LINK_BASE_CONFIGURATION_VALUE */
