#include "Chicane/Runtime/Renderer/View/Plane.hpp"

namespace Chicane
{
    namespace View
    {
        bool Plane::contains(const Vec<3, float>& inPoint) const
        {
            return distanceToPoint(inPoint) >= -FLT_EPSILON;
        }

        void Plane::update(const Vec<3, float>& inNormal, const Vec<3, float>& inPoint)
        {
            m_normal   = glm::normalize(inNormal);
            m_distance = -m_normal.dot(inPoint);
        }

        float Plane::distanceToPoint(const Vec<3, float>& inPoint) const
        {
            return m_normal.dot(inPoint) - m_distance;
        }
    }
}