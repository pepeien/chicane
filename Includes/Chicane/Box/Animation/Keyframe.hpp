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
            AnimationKeyframe();

        public:
            Time      time;
            Transform transform;
            String    easing;
        };
    }
}
