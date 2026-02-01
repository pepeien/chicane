#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec2.hpp"

namespace Chicane
{
    struct CHICANE_CORE Bounds2D
    {
    public:
        Bounds2D();

    public:
        bool contains(const Vec2& inPoint) const;

        bool overlaps(const Bounds2D& inOther) const;

    public:
        float top;
        float bottom;
        float left;
        float right;
    };
}