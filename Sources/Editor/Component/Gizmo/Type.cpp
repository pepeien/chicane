#include "Editor/Component/Gizmo/Type.hpp"
#include "Editor/Component/Gizmo/Type.reflected.hpp"

namespace Editor
{
    Chicane::String toString(Editor::GizmoType inValue)
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
