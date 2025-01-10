#ifndef SRC_FULLY_SUPPORTED_SIGNAL_VALUE
#define SRC_FULLY_SUPPORTED_SIGNAL_VALUE


#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include <complex>

namespace PySysLinkBase
{    
    using FullySupportedSignalValue = std::variant<
        int,
        double,
        bool,
        std::complex<double>,
        std::string>;


} // namespace PySysLinkBase



#endif /* SRC_FULLY_SUPPORTED_SIGNAL_VALUE */
