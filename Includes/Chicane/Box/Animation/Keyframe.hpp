#pragma once

#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/String.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX AnimationKeyframe
        {
        public:
            using List = std::vector<AnimationKeyframe>;

        public:
            static constexpr inline const char* TAG                   = "Keyframe";
            static constexpr inline const char* TIME_ATTRIBUTE_NAME   = "time";
            static constexpr inline const char* EASING_ATTRIBUTE_NAME = "easing";

        public:
            AnimationKeyframe();

        public:
            Time      time;
            Transform transform;
            String    easing;
        };
    }
}
