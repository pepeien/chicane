#include "Editor/Component/Gizmo/Axis.hpp"
#include "Editor/Component/Gizmo/Axis.reflected.hpp"

namespace Editor
{
    Chicane::String toString(Editor::GizmoAxis inValue)
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
