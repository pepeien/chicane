#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math.hpp"

namespace Chicane
{
    namespace View
    {
        struct CHICANE Plane
        {
        public:
            bool contains(const Vec<3, float>& inPoint) const;
            void update(const Vec<3, float>& inNormal, const Vec<3, float>& inPoint);

        private:
            float distanceToPoint(const Vec<3, float>& inPoint) const;

        private:
            float         m_distance = 0.0f;
            Vec<3, float> m_normal   = Vec3Zero;
        };
    }
}