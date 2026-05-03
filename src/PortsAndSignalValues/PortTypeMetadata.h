#ifndef PORT_TYPE_METADATA_H
#define PORT_TYPE_METADATA_H

#include <string>
#include <optional>
#include <vector>

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

    std::vector<PortTypeMetadata> ParsePortTypeMetadatas(const std::vector<std::string>& portTypeStrs);
}

#endif