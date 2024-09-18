#include "Chicane/Renderer/Camera/Frustum/Sphere.hpp"

#include "Chicane/Game/Transformable.hpp"
#include "Chicane/Game/Actor/Component/Camera.hpp"

namespace Chicane
{
    namespace Camera
    {
        bool SphereFrustum::contains(const Transformable* inSubject) const
        {
            const Vec<3, float>& subjectCenter = inSubject->getCenter();

        	return (
        		isWithinPlane(m_top,    subjectCenter) &&
        		isWithinPlane(m_bottom, subjectCenter) &&
                isWithinPlane(m_left,   subjectCenter) &&
        		isWithinPlane(m_right,  subjectCenter) &&
        		isWithinPlane(m_near,   subjectCenter) &&
        		isWithinPlane(m_far,    subjectCenter)
            );
        };

        void SphereFrustum::use(const CameraComponent* inCamera)
        {
            const Vec<3, float>& up          = inCamera->getUp();
            const Vec<3, float>& left        = inCamera->getLeft();
            const Vec<3, float>& forward     = inCamera->getForward();
            const Vec<3, float>& translation = inCamera->getTranslation();

            float aspectRatio = inCamera->getAspectRatio();
            float fieldOfView = inCamera->getFieldOfView();
            float farClip     = inCamera->getFarClip();
            float nearClip    = inCamera->getNearClip();

            const float halfVertical   = farClip * tanf(fieldOfView * 0.5f);
            const float halfHorizontal = halfVertical * aspectRatio;

            const Vec<3, float> forwardFar = farClip * forward;

            center = inCamera->getCenter();

            m_top.update(
                glm::cross(left, forwardFar - up * halfVertical),
                translation
            );
            m_bottom.update(
                glm::cross(forwardFar + up * halfVertical, left),
                translation
            );
            m_left.update(
                glm::cross(up, forwardFar + left * halfHorizontal),
                translation
            );
            m_right.update(
                glm::cross(forwardFar - left * halfHorizontal, up),
                translation
            );
            m_near.update(
                forward,
                translation + nearClip * forward
            );
            m_far.update(
                -forward,
                translation + forwardFar
            );
        }

        bool SphereFrustum::isWithinPlane(
            const Plane& inPlane,
            const Vec<3, float> inPoint
        ) const
        {
        	return inPlane.getDistance(inPoint) > -radius;
        }
    }
}