#pragma once

#include <cstdint>

#include "Chicane/Drift.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_DRIFT String toString(Drift::Easing inValue);
}
