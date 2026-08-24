#include "Chicane/Drift/Keyframe.hpp"

#include <algorithm>

namespace Chicane
{
    namespace Drift
    {
        Keyframe::Keyframe()
            : time(0.0f),
              value({}),
              easing(EasingCurve::ease())
        {}

        Keyframe::Keyframe(float inTime, const std::vector<float>& inValue)
            : Keyframe(inTime, inValue, EasingCurve::ease())
        {}

        Keyframe::Keyframe(float inTime, const std::vector<float>& inValue, const EasingCurve& inEasing)
            : time(std::max(0.0f, inTime)),
              value(inValue),
              easing(inEasing)
        {}
    }
}
