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

    inline String toString(Drift::Easing inValue)
    {
        switch (inValue)
        {
        case Drift::Easing::Linear:
            return "Linear";

        case Drift::Easing::Ease:
            return "Ease";

        case Drift::Easing::EaseIn:
            return "EaseIn";

        case Drift::Easing::EaseOut:
            return "EaseOut";

        case Drift::Easing::EaseInOut:
            return "EaseInOut";

        case Drift::Easing::CubicBezier:
            return "CubicBezier";

        default:
            return "";
        }
    }
}
