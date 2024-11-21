#ifndef SRC_PY_SYS_LINK_BASE_CONFIGURATION_VALUE
#define SRC_PY_SYS_LINK_BASE_CONFIGURATION_VALUE

#include <string>
#include <variant>
#include <vector>
#include <memory>

namespace PySysLinkBase
{
    using ConfigurationValuePrimitive  = std::variant<
        int,
        double,
        bool,
        std::string>;
    
    using ConfigurationValue = std::variant<
        ConfigurationValuePrimitive,
        std::vector<ConfigurationValuePrimitive>>;
} // namespace PySysLinkBase



#endif /* SRC_PY_SYS_LINK_BASE_CONFIGURATION_VALUE */
