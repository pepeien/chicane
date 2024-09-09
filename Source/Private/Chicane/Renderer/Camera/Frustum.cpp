#include "Chicane/Renderer/Camera/Frustum.hpp"

namespace Chicane
{
    namespace Camera
    {
        void Plane::update(const Vec<3, float>& inNormal, const Vec<3, float>& inPoint)
        {
            setNormal(inNormal);
            setDistance(inPoint);
        }

        void Plane::setDistance(const Vec<3, float>& inPoint)
        {
            distance = normal.dot(inPoint);
        }

        void Plane::setNormal(const Vec<3, float>& inNormal)
        {
            normal = glm::normalize(inNormal);
        }

        float Plane::getSignedDistanceToPlan(const Vec<3, float>& inPoint) const
        {
        	return normal.dot(inPoint) - distance;
        }
    }
}