#pragma once

#include <Chicane/Core/Math/Vec/Vec3.hpp>

#include "Editor/Component/Gizmo/Axis.hpp"

namespace Editor
{
    struct PlaneHandle
    {
        GizmoAxis     axis = GizmoAxis::None;
        Chicane::Vec3 u    = {};
        Chicane::Vec3 v    = {};
        Chicane::Vec3 n    = {};
    };
}
