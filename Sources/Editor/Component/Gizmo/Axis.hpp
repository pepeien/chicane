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

    Chicane::String toString(Editor::GizmoAxis inValue);

    inline bool isPlaneAxis(GizmoAxis inAxis)
    {
        return inAxis == GizmoAxis::XY || inAxis == GizmoAxis::XZ || inAxis == GizmoAxis::YZ;
    }
}
