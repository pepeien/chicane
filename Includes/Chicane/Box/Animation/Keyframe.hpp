#pragma once

#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/String.hpp"

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
            float     time;
            Transform transform;
            String    easing;
        };
    }
}
