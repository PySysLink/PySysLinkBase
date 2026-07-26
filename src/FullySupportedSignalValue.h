#ifndef SRC_FULLY_SUPPORTED_SIGNAL_VALUE
#define SRC_FULLY_SUPPORTED_SIGNAL_VALUE


#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include <complex>
#include <Eigen/Dense>

#include "PortsAndSignalValues/UnknownTypeSignalValue.h"
#include "PortsAndSignalValues/SignalValue.h"

namespace PySysLinkBase
{    
    using IntMatrix     = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;
    using DoubleMatrix  = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
    using BoolMatrix    = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic>;
    using ComplexMatrix = Eigen::Matrix<std::complex<double>, Eigen::Dynamic, Eigen::Dynamic>;

    using FullySupportedSignalValue = std::variant<
        int,
        double,
        bool,
        std::complex<double>,
        std::string,
        IntMatrix,
        DoubleMatrix,
        BoolMatrix,
        ComplexMatrix
    >;

    FullySupportedSignalValue ConvertToFullySupportedSignalValue(const std::shared_ptr<UnknownTypeSignalValue>& unknownValue);


    std::string FullySupportedSignalValueToString(const FullySupportedSignalValue& value);

} // namespace PySysLinkBase



#endif /* SRC_FULLY_SUPPORTED_SIGNAL_VALUE */
