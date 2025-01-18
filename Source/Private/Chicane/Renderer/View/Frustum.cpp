#include "Chicane/Renderer/View/Frustum.hpp"

namespace Chicane
{
    namespace View
    {
        void Plane::update(const Vec<3, float>& inNormal, const Vec<3, float>& inPoint)
        {
            normal   = glm::normalize(inNormal);
            distance = normal.dot(inPoint);
        }

        float Plane::getDistance(const Vec<3, float>& inPoint) const
        {
        	return normal.dot(inPoint) - distance;
        }
    }
}