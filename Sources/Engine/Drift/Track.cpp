#include "Chicane/Drift/Track.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Drift/Interpolation.hpp"

namespace Chicane
{
    namespace Drift
    {
        Track::Track()
            : name(""),
              keyframes({})
        {}

        Track::Track(const String& inName)
            : name(inName),
              keyframes({})
        {}

        void Track::addKeyframe(const Keyframe& inValue)
        {
            keyframes.push_back(inValue);

            std::sort(
                keyframes.begin(),
                keyframes.end(),
                [](const Keyframe& inLeft, const Keyframe& inRight) { return inLeft.time < inRight.time; }
            );
        }

        void Track::addKeyframe(float inTime, const std::vector<float>& inValue)
        {
            addKeyframe(Keyframe(inTime, inValue));
        }

        void Track::addKeyframe(float inTime, const std::vector<float>& inValue, const EasingCurve& inEasing)
        {
            addKeyframe(Keyframe(inTime, inValue, inEasing));
        }

        std::vector<float> Track::sample(float inTime) const
        {
            if (keyframes.empty())
            {
                return {};
            }

            if (keyframes.size() == 1 || inTime <= keyframes.front().time)
            {
                return keyframes.front().value;
            }

            if (inTime >= keyframes.back().time)
            {
                return keyframes.back().value;
            }

            for (std::size_t i = 1; i < keyframes.size(); i++)
            {
                const Keyframe& next = keyframes.at(i);

                if (inTime > next.time)
                {
                    continue;
                }

                const Keyframe& previous = keyframes.at(i - 1);
                const float     span     = next.time - previous.time;

                if (span <= 0.0f)
                {
                    return next.value;
                }

                const float progress = previous.easing.evaluate((inTime - previous.time) / span);

                return mix(previous.value, next.value, progress);
            }

            return keyframes.back().value;
        }
    }
}
