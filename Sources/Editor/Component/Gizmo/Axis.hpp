#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_ENUM()
    enum class GizmoAxis : std::uint8_t
    {
        None,
        X,
        Y,
        Z,
        XY,
        XZ,
        YZ,
        Center
    };

    inline Chicane::String toString(Editor::GizmoAxis inValue)
    {
        switch (inValue)
        {
        case GizmoAxis::None:
            return "None";

        case GizmoAxis::X:
            return "X";

        case GizmoAxis::Y:
            return "Y";

        case GizmoAxis::Z:
            return "Z";

        case GizmoAxis::XY:
            return "XY";

        case GizmoAxis::XZ:
            return "XZ";

        case GizmoAxis::YZ:
            return "YZ";

        case GizmoAxis::Center:
            return "Center";

        default:
            return "";
        }
    }
}
