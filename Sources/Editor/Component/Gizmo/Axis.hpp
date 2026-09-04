#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>

namespace Editor
{
    CH_ENUM()
    enum class GizmoAxis : std::uint8_t
    {
        None,
        X,
        Y,
        Z,
        Center
    };
}
