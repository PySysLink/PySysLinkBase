#ifndef SRC_FULLY_SUPPORTED_SIGNAL_VALUE
#define SRC_FULLY_SUPPORTED_SIGNAL_VALUE


#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include <complex>

#include "PortsAndSignalValues/UnknownTypeSignalValue.h"
#include "PortsAndSignalValues/SignalValue.h"

namespace PySysLinkBase
{    
    using FullySupportedSignalValue = std::variant<
    int,
    double,
    bool,
    std::complex<double>,
    std::string
>;

FullySupportedSignalValue ConvertToFullySupportedSignalValue(const std::shared_ptr<UnknownTypeSignalValue>& unknownValue);

} // namespace PySysLinkBase



#endif /* SRC_FULLY_SUPPORTED_SIGNAL_VALUE */
