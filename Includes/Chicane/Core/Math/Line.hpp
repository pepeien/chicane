#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct CHICANE_CORE Line
    {
    public:
        Vec<2, float> start = Vec2Zero;
        Vec<2, float> end   = Vec2Zero;
    };
}