#include "Chicane/Core/View/Plane.hpp"

namespace Chicane
{
    bool ViewPlane::contains(const Vec3& inPoint) const
    {
        return distanceToPoint(inPoint) >= -FLT_EPSILON;
    }

    bool ViewPlane::contains(const Vec3& inMin, const Vec3& inMax) const
    {
        const Vec3 p(
            m_normal.x >= 0.0f ? inMax.x : inMin.x,
            m_normal.y >= 0.0f ? inMax.y : inMin.y,
            m_normal.z >= 0.0f ? inMax.z : inMin.z
        );

        return contains(p);
    }

    void ViewPlane::setNormal(const Vec3& inFirstEdge, const Vec3& inSecondEdge, const Vec3& inPosition)
    {
        const Vec3 firstEdge  = inFirstEdge - inPosition;
        const Vec3 secondEdge = inSecondEdge - inPosition;

        setNormal(firstEdge.cross(secondEdge).normalize(), inPosition);
    }

    void ViewPlane::setNormal(const Vec3& inValue, const Vec3& inPosition)
    {
        m_normal   = inValue.normalize();
        m_distance = -m_normal.dot(inPosition);
    }

    float ViewPlane::distanceToPoint(const Vec3& inPoint) const
    {
        return m_normal.dot(inPoint) + m_distance;
    }
}