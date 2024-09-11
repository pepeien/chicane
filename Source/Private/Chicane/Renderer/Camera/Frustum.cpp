#include "Chicane/Renderer/Camera/Frustum.hpp"

namespace Chicane
{
    namespace Camera
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