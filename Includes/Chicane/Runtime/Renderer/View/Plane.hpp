#pragma once

#include "Chicane/Runtime/Essential.hpp"

namespace Chicane
{
    namespace View
    {
        struct CHICANE_RUNTIME Plane
        {
        public:
            bool contains(const Vec3& inPoint) const;
            void update(const Vec3& inNormal, const Vec3& inPoint);

        private:
            float distanceToPoint(const Vec3& inPoint) const;

        private:
            float m_distance = 0.0f;
            Vec3  m_normal   = Vec3::Zero;
        };
    }
}