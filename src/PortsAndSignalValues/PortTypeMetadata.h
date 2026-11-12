#ifndef PORT_TYPE_METADATA_H
#define PORT_TYPE_METADATA_H

#include <string>
#include <optional>
#include <vector>
#include <complex>
#include <map>
#include <string>
#include "../FullySupportedSignalValue.h"

namespace PySysLinkBase
{
    enum class PortCategory
    {
        FullySupportedSignalValue,
        Enumeration,
        Structure,
        PointerToObject,
        OtherType,
        Inherited,
        Unknown
    };

    struct PortTypeMetadata
    {
        PortCategory category = PortCategory::Unknown;

        std::optional<std::string> signalValueType;
        std::optional<std::string> enumerationName;
        std::optional<std::string> structureName;
        std::optional<std::string> pointingObjectClassName;
        std::optional<std::string> otherTypeName;

        int inheritanceGroup = 0;
    };

    template<typename F>
    auto DispatchType(const std::string& type, F&& f)
    {
        if (type == "int")
            return f.template operator()<int>();

        if (type == "double")
            return f.template operator()<double>();

        if (type == "complex_double")
            return f.template operator()<std::complex<double>>();

        if (type == "bool")
            return f.template operator()<bool>();

        if (type == "matrix<int>")
            return f.template operator()<IntMatrix>();

        if (type == "matrix<double>")
            return f.template operator()<DoubleMatrix>();

        if (type == "matrix<bool>")
            return f.template operator()<BoolMatrix>();

        if (type == "matrix<complex_double>")
            return f.template operator()<ComplexMatrix>();

        throw std::invalid_argument("Unsupported type: " + type);
    }

    std::string PortCategoryToString(PortCategory cat);
    std::vector<PortTypeMetadata> ParsePortTypeMetadatas(const std::vector<std::string>& portTypeStrs);
}

#endif