#include "Chicane/Runtime/Renderer/View/Plane.hpp"

namespace Chicane
{
    namespace View
    {
        bool Plane::contains(const Vec3& inPoint) const
        {
            return distanceToPoint(inPoint) >= -FLT_EPSILON;
        }

        void Plane::update(const Vec3& inNormal, const Vec3& inPoint)
        {
            m_normal   = glm::normalize(inNormal);
            m_distance = -m_normal.dot(inPoint);
        }

        float Plane::distanceToPoint(const Vec3& inPoint) const
        {
            return m_normal.dot(inPoint) - m_distance;
        }
    }
}