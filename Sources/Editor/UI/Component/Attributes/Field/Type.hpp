#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_ENUM()
    enum class AttributeFieldType : std::uint8_t
    {
        Text,
        Bool,
        Enum,
        Vec3,
        Float,
        Color,
        Asset
    };

    inline Chicane::String toString(AttributeFieldType inValue)
    {
        switch (inValue)
        {
        case AttributeFieldType::Text:
            return "Text";

        case AttributeFieldType::Bool:
            return "Bool";

        case AttributeFieldType::Enum:
            return "Enum";

        case AttributeFieldType::Vec3:
            return "Vec3";

        case AttributeFieldType::Float:
            return "Float";

        case AttributeFieldType::Color:
            return "Color";

        case AttributeFieldType::Asset:
            return "Asset";

        default:
            return "";
        }
    }
}
