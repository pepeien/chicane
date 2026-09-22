#include "Chicane/Drift/Easing.hpp"

namespace Chicane
{
    String toString(Drift::Easing inValue)
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
