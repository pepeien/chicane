#pragma once

#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Vertex
    {
    public:
        Vec<3, float> position = Vec3Zero;
        Vec<4, float> color    = Vec4Zero;
        Vec<2, float> uv       = Vec2Zero;
        Vec<3, float> normal   = Vec3Zero;
    };
}