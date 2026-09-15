#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>

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
}
