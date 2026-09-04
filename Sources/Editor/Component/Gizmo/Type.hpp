#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>

namespace Editor
{
    CH_ENUM()
    enum class GizmoType : std::uint8_t
    {
        Translation,
        Rotation,
        Scale
    };
}
