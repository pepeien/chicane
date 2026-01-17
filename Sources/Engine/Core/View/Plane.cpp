#include "Chicane/Core/View/Plane.hpp"

namespace Chicane
{
    bool ViewPlane::contains(const Vec3& inPoint) const
    {
        return distanceToPoint(inPoint) >= -FLT_EPSILON;
    }

    void ViewPlane::update(const Vec3& inNormal, const Vec3& inPoint)
    {
        m_normal   = inNormal.normalize();
        m_distance = -m_normal.dot(inPoint);
    }

    float ViewPlane::distanceToPoint(const Vec3& inPoint) const
    {
        return m_normal.dot(inPoint) - m_distance;
    }
}