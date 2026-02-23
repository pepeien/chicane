#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    struct CHICANE_CORE BoundsCorner
    {
    public:
        BoundsCorner(const Vec3& inBase, const Vec3& inTransformed)
            : base(inBase),
              transformed(inTransformed)
        {}

        BoundsCorner()
            : base(Vec3::Zero),
              transformed(Vec3::Zero)
        {}

    public:
        Vec3 base;
        Vec3 transformed;
    };
}