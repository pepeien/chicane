#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace View
    {
        struct Plane
        {
        public:
            float getDistance(const Vec<3, float>& inPoint) const;

            void update(const Vec<3, float>& inNormal, const Vec<3, float>& inPoint);

        public:
            float         distance = 0.0f;
            Vec<3, float> normal   = {};
        };
    }
}