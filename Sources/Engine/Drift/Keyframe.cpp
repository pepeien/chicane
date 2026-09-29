#include "Chicane/Drift/Keyframe.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Drift
    {
        Keyframe::Keyframe()
            : time(Time::sZero()),
              value({}),
              easing(EasingCurve::sEase())
        {}

        Keyframe::Keyframe(const Time& inTime, const std::vector<float>& inValue)
            : Keyframe(inTime, inValue, EasingCurve::sEase())
        {}

        Keyframe::Keyframe(const Time& inTime, const std::vector<float>& inValue, const EasingCurve& inEasing)
            : time(inTime < Time::sZero() ? Time::sZero() : inTime),
              value(inValue),
              easing(inEasing)
        {}
    }
}
