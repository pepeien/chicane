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

        inline String toString(Easing inValue)
        {
            switch (inValue)
            {
            case Easing::Linear:
                return "Linear";

            case Easing::Ease:
                return "Ease";

            case Easing::EaseIn:
                return "EaseIn";

            case Easing::EaseOut:
                return "EaseOut";

            case Easing::EaseInOut:
                return "EaseInOut";

            case Easing::CubicBezier:
                return "CubicBezier";

            default:
                return "";
            }
        }
    }
}
