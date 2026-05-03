#include "PortTypeMetadata.h"
#include <sstream>
#include <iostream>

namespace PySysLinkBase
{

static bool starts_with(const std::string& s, const std::string& prefix)
{
    return s.rfind(prefix, 0) == 0;
}

std::vector<PortTypeMetadata> ParsePortTypeMetadatas(const std::vector<std::string>& portTypeStrs)
{
    std::vector<PortTypeMetadata> portTypes;

    for (const auto& str : portTypeStrs)
    {
        PortTypeMetadata meta;

        std::cout << "Parsing port type metadata from string: " << str << std::endl;

        // -------------------------------
        // FullySupportedSignalValue
        // -------------------------------
        if (starts_with(str, "FullySupportedSignalValue."))
        {
            meta.category = PortCategory::FullySupportedSignalValue;
            meta.signalValueType = str.substr(std::string("FullySupportedSignalValue.").size());
            std::cout << "Parsed a FullySupportedSignalValue port type with signal value type: " << meta.signalValueType.value() << std::endl;
        }
        else if (str == "FullySupportedSignalValueType.Any")
        {
            meta.category = PortCategory::FullySupportedSignalValue;
            meta.signalValueType = std::nullopt;
            std::cout << "Parsed a FullySupportedSignalValue port type with any signal value type." << std::endl;
        }

        // -------------------------------
        // Enumeration
        // -------------------------------
        else if (starts_with(str, "Enumeration:"))
        {
            meta.category = PortCategory::Enumeration;
            meta.enumerationName = str.substr(std::string("Enumeration:").size());
            std::cout << "Parsed a Enumeration port type with enumeration name: " << meta.enumerationName.value() << std::endl;
        }

        // -------------------------------
        // Structure
        // -------------------------------
        else if (starts_with(str, "Structure:"))
        {
            meta.category = PortCategory::Structure;
            meta.structureName = str.substr(std::string("Structure:").size());
            std::cout << "Parsed a Structure port type with structure name: " << meta.structureName.value() << std::endl;
        }

        // -------------------------------
        // PointerToObject
        // -------------------------------
        else if (starts_with(str, "PointerToObject:"))
        {
            meta.category = PortCategory::PointerToObject;
            meta.pointingObjectClassName = str.substr(std::string("PointerToObject:").size());
            std::cout << "Parsed a PointerToObject port type with pointing object class name: " << meta.pointingObjectClassName.value() << std::endl;
        }

        // -------------------------------
        // OtherType
        // -------------------------------
        else if (starts_with(str, "OtherType:"))
        {
            meta.category = PortCategory::OtherType;
            meta.otherTypeName = str.substr(std::string("OtherType:").size());
            std::cout << "Parsed an OtherType port type with other type name: " << meta.otherTypeName.value() << std::endl;
        }

        // -------------------------------
        // Inherited
        // -------------------------------
        else if (starts_with(str, "Inherited"))
        {
            meta.category = PortCategory::Inherited;

            // Optional: parse group or supported types later
            // For now, just mark it as inherited
            std::cout << "Parsed an Inherited port type." << std::endl;
        }

        else
        {
            throw std::invalid_argument("Unsupported port type string: " + str);
        }

        portTypes.push_back(meta);
    }

    return portTypes;
}

}