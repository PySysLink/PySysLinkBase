#ifndef SRC_BLOCK_TYPES_BASIC_CPP_CONFIGURATION_VALUE
#define SRC_BLOCK_TYPES_BASIC_CPP_CONFIGURATION_VALUE


#include <string>
#include <variant>
#include <vector>
#include <memory>

namespace BlockTypes::BasicCpp
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
        ConfigurationValuePrimitive>;
} // namespace BlockTypes::BasicCpp



#endif 
