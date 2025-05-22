#include "Chicane/Runtime/Renderer/View/Frustum.hpp"

#include "Chicane/Runtime/Game/Transformable/Instance.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/View.hpp"

namespace Chicane
{
    namespace View
    {
        bool Frustum::contains(const Transformable* inSubject) const
        {
            const std::vector<Plane> planes = { m_top, m_bottom, m_near, m_far, m_right, m_left };
            const std::vector<Vec3>& corners = inSubject->getBounds().getCorners();

            for (const Plane& plane : planes)
            {
                bool isFullyOutside = true;

                for (const Vec3& corner : corners)
                {
                    if (plane.contains(corner))
                    {
                        isFullyOutside = false;

                        break;
                    }
                }

                if (isFullyOutside)
                {
                    return false;
                }
            }

            return true;
        }

        void Frustum::update(const CView* inView)
        {
            const Vec3& up          = inView->getUp();
            const Vec3& right       = inView->getRight();
            const Vec3& forward     = inView->getForward();
            const Vec3& translation = inView->getTranslation();

            const float aspectRatio = inView->getAspectRatio();
            const float fieldOfView = inView->getFieldOfView();
            const float nearClip    = inView->getNearClip();
            const float farClip     = inView->getFarClip();

            const float halfVertical   = farClip * tanf(fieldOfView * 0.5f);
            const float halfHorizontal = halfVertical * aspectRatio;

            const Vec3 nearCenter = translation + (nearClip * forward);
            const Vec3 farCenter = translation + (farClip * forward);

            m_near.update(-forward, nearCenter);
            m_far.update(forward, farCenter);
            m_top.update(up, farCenter + up * halfVertical);
            m_bottom.update(-up, farCenter - up * halfVertical);
            m_right.update(right, farCenter + right * halfHorizontal);
            m_left.update(-right, farCenter - right * halfHorizontal);
        }
    }
}