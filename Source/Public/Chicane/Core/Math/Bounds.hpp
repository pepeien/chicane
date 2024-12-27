#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct Bounds
    {
    public:
        bool contains(const Bounds& inBounds) const;
        bool contains(const Vec<3, float>& inPoint) const;

    public:
        Vec<3, float> extent = Vec3Zero;

        Vec<3, float> top    = Vec3Zero;
        Vec<3, float> origin = Vec3Zero;
        Vec<3, float> center = Vec3Zero;
    };
}