#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE ViewPlane
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