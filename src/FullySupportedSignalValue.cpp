#include "FullySupportedSignalValue.h"

namespace PySysLinkBase
{
    FullySupportedSignalValue ConvertToFullySupportedSignalValue(const std::shared_ptr<UnknownTypeSignalValue>& unknownValue)
    {
        try {
            if (auto intValue = unknownValue->TryCastToTyped<int>()) {
                return FullySupportedSignalValue(intValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto doubleValue = unknownValue->TryCastToTyped<double>()) {
                return FullySupportedSignalValue(doubleValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto boolValue = unknownValue->TryCastToTyped<bool>()) {
                return FullySupportedSignalValue(boolValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto complexValue = unknownValue->TryCastToTyped<std::complex<double>>()) {
                return FullySupportedSignalValue(complexValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto stringValue = unknownValue->TryCastToTyped<std::string>()) {
                return FullySupportedSignalValue(stringValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto stringValue = unknownValue->TryCastToTyped<IntMatrix>()) {
                return FullySupportedSignalValue(stringValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto stringValue = unknownValue->TryCastToTyped<DoubleMatrix>()) {
                return FullySupportedSignalValue(stringValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto stringValue = unknownValue->TryCastToTyped<BoolMatrix>()) {
                return FullySupportedSignalValue(stringValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        try {
            if (auto stringValue = unknownValue->TryCastToTyped<ComplexMatrix>()) {
                return FullySupportedSignalValue(stringValue->GetPayload());
            }
        } catch (const std::bad_cast&) {}

        throw std::runtime_error("UnknownTypeSignalValue cannot be converted to FullySupportedSignalValue");
    }


    template<typename T>
    std::string ToStringImpl(const T& value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    inline std::string ToStringImpl(const bool& value)
    {
        return value ? "true" : "false";
    }

    inline std::string ToStringImpl(const std::complex<double>& value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    inline std::string ToStringImpl(const Eigen::MatrixXd& m)
    {
        std::ostringstream oss;
        oss << m;
        return oss.str();
    }

    inline std::string ToStringImpl(const Eigen::MatrixXcd& m)
    {
        std::ostringstream oss;
        oss << m;
        return oss.str();
    }

    std::string FullySupportedSignalValueToString(
            const FullySupportedSignalValue& value)
    {
        return std::visit(
            [](const auto& v)
            {
                return ToStringImpl(v);
            },
            value);
    }

} // namespace PySysLinkBase