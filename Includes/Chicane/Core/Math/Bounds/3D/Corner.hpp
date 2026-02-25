#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE Bounds3DCorner
    {
    public:
        Bounds3DCorner(const Vec3& inBase, const Vec3& inTransformed);
        Bounds3DCorner();

    public:
        Vec3 base;
        Vec3 transformed;
    };
}