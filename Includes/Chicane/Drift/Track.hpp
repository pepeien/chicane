#pragma once

#include <vector>

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Keyframe.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT Track
        {
        public:
            using List = std::vector<Track>;

        public:
            Track();
            Track(const String& inName);

        public:
            void addKeyframe(const Keyframe& inValue);
            void addKeyframe(float inTime, const std::vector<float>& inValue);
            void addKeyframe(float inTime, const std::vector<float>& inValue, const EasingCurve& inEasing);

            std::vector<float> sample(float inTime) const;

        public:
            String        name;
            Keyframe::List keyframes;
        };
    }
}
