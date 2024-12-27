#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"

namespace Chicane
{
    struct Direction
    {
        Vec<3, float> forward = FORWARD_DIRECTION;
        Vec<3, float> up      = UP_DIRECTION;
        Vec<3, float> right   = RIGHT_DIRECTION;
    };
}