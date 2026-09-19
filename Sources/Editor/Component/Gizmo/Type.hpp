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

    inline Chicane::String toString(Editor::GizmoType inValue)
    {
        switch (inValue)
        {
        case GizmoType::Translation:
            return "Translation";

        case GizmoType::Rotation:
            return "Rotation";

        case GizmoType::Scale:
            return "Scale";

        default:
            return "";
        }
    }
}
