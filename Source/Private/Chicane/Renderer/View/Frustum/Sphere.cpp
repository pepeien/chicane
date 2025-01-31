#include "Chicane/Renderer/View/Frustum/Sphere.hpp"

#include "Chicane/Game/Transformable/Instance.hpp"
#include "Chicane/Game/Transformable/Component/View.hpp"

namespace Chicane
{
    namespace View
    {
        bool SphereFrustum::contains(const Transformable* inSubject) const
        {
            const Vec<3, float>& center = inSubject->getCenter();

        	return (
        		isWithinPlane(m_top,    center) &&
        		isWithinPlane(m_bottom, center) &&
                isWithinPlane(m_left,   center) &&
        		isWithinPlane(m_right,  center) &&
        		isWithinPlane(m_near,   center) &&
        		isWithinPlane(m_far,    center)
            );
        };

        void SphereFrustum::use(const CView* inView)
        {
            const Vec<3, float>& up          = inView->getUp();
            const Vec<3, float>& right       = inView->getRight();
            const Vec<3, float>& forward     = inView->getForward();
            const Vec<3, float>& translation = inView->getTranslation();

            float aspectRatio = inView->getAspectRatio();
            float fieldOfView = inView->getFieldOfView();
            float farClip     = inView->getFarClip();
            float nearClip    = inView->getNearClip();

            const float halfVertical   = farClip * tanf(fieldOfView * 0.5f);
            const float halfHorizontal = halfVertical * aspectRatio;

            const Vec<3, float> forwardFar = farClip * forward;

            center = inView->getCenter();

            m_top.update(
                glm::cross(right, forwardFar - up * halfVertical),
                translation
            );
            m_bottom.update(
                glm::cross(forwardFar + up * halfVertical, right),
                translation
            );
            m_left.update(
                glm::cross(up, forwardFar + right * halfHorizontal),
                translation
            );
            m_right.update(
                glm::cross(forwardFar - right * halfHorizontal, up),
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

        bool SphereFrustum::isWithinPlane(const Plane& inPlane, const Vec<3, float>& inPoint) const
        {
        	return inPlane.getDistance(inPoint) > -radius;
        }
    }
}