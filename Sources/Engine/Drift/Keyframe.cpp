#include "Chicane/Drift/Keyframe.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Drift
    {
        Keyframe::Keyframe()
            : time(Time::Zero()),
              value({}),
              easing(EasingCurve::ease())
        {}

        Keyframe::Keyframe(const Time& inTime, const std::vector<float>& inValue)
            : Keyframe(inTime, inValue, EasingCurve::ease())
        {}

        Keyframe::Keyframe(const Time& inTime, const std::vector<float>& inValue, const EasingCurve& inEasing)
            : time(inTime < Time::Zero() ? Time::Zero() : inTime),
              value(inValue),
              easing(inEasing)
        {}
    }
}
