#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"

namespace Chicane
{
    namespace Drift
    {
        enum class Easing : std::uint8_t
        {
            Linear,
            Ease,
            EaseIn,
            EaseOut,
            EaseInOut,
            CubicBezier
        };
    }
}
