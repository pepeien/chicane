#pragma once

#include <vector>

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Easing.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT Keyframe
        {
        public:
            using List = std::vector<Keyframe>;

        public:
            Keyframe();
            Keyframe(float inTime, const std::vector<float>& inValue);
            Keyframe(float inTime, const std::vector<float>& inValue, const EasingCurve& inEasing);

        public:
            float              time;
            std::vector<float> value;
            EasingCurve        easing;
        };
    }
}
