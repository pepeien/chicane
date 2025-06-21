#include "Chicane/Runtime/Renderer/View/Plane.hpp"

namespace Chicane
{
    bool RendererViewPlane::contains(const Vec3& inPoint) const
    {
        return distanceToPoint(inPoint) >= -FLT_EPSILON;
    }

    void RendererViewPlane::update(const Vec3& inNormal, const Vec3& inPoint)
    {
        m_normal   = glm::normalize(inNormal);
        m_distance = -m_normal.dot(inPoint);
    }

    float RendererViewPlane::distanceToPoint(const Vec3& inPoint) const
    {
        return m_normal.dot(inPoint) - m_distance;
    }
}