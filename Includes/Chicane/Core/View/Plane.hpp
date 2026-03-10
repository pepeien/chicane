#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math/Vec/Vec3.hpp"

namespace Chicane
{
    struct CHICANE_CORE ViewPlane
    {
    public:
        bool contains(const Vec3& inPoint) const;

        void setNormal(const Vec3& inFirstEdge, const Vec3& inSecondEdge, const Vec3& inPosition);
        void setNormal(const Vec3& inNormal, const Vec3& inPosition);

    private:
        float distanceToPoint(const Vec3& inPoint) const;

    private:
        float m_distance = 0.0f;
        Vec3  m_normal   = Vec3::Zero();
    };
}