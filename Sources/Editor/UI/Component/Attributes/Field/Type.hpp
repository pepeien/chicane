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

    Chicane::String toString(AttributeFieldType inValue);
}
