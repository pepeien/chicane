#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_ENUM()
    enum class GizmoType : std::uint8_t
    {
        Translation,
        Rotation,
        Scale
    };

    Chicane::String toString(Editor::GizmoType inValue);
}
