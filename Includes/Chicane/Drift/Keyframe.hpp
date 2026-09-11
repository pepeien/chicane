#pragma once

#include <vector>

#include "Chicane/Core/Time.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Easing/Curve.hpp"

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
            Keyframe(const Time& inTime, const std::vector<float>& inValue);
            Keyframe(const Time& inTime, const std::vector<float>& inValue, const EasingCurve& inEasing);

        public:
            Time               time;
            std::vector<float> value;
            EasingCurve        easing;
        };
    }
}
