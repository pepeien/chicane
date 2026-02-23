#include "Chicane/Core/View/Frustum.hpp"

namespace Chicane
{
    bool ViewFrustum::contains(const Transformable* inSubject) const
    {
        const std::vector<ViewPlane> planes  = {m_top, m_bottom, m_near, m_far, m_right, m_left};
        const Vertex::List&          corners = inSubject->getBounds().getCorners();

        for (const ViewPlane& plane : planes)
        {
            bool bIsFullyOutside = true;

            for (const Vertex& corner : corners)
            {
                if (plane.contains(corner.position))
                {
                    bIsFullyOutside = false;

                    break;
                }
            }

            if (bIsFullyOutside)
            {
                return false;
            }
        }

        return true;
    }

    void ViewFrustum::update(const Transformable* inView, const ViewSettings& inSettings)
    {
        const Rotator& rotation    = inView->getRotation();
        const Vec3&    translation = inView->getTranslation();

        const Vec3 up      = rotation.getUp();
        const Vec3 right   = rotation.getRight();
        const Vec3 forward = rotation.getForward();

        const float aspectRatio = inSettings.aspectRatio;
        const float fieldOfView = inSettings.fieldOfView;
        const float nearClip    = inSettings.nearClip;
        const float farClip     = inSettings.farClip;

        const float halfVertical   = farClip * tanf(fieldOfView * 0.5f);
        const float halfHorizontal = halfVertical * aspectRatio;

        const Vec3 nearCenter = translation + forward * nearClip;
        const Vec3 farCenter  = translation + forward * farClip;

        m_near.setNormal(forward, nearCenter);
        m_far.setNormal(-forward, farCenter);

        const Vec3 farTopRight    = farCenter + up * halfVertical + right * halfHorizontal;
        const Vec3 farTopLeft     = farCenter + up * halfVertical - right * halfHorizontal;
        const Vec3 farBottomRight = farCenter - up * halfVertical + right * halfHorizontal;
        const Vec3 farBottomLeft  = farCenter - up * halfVertical - right * halfHorizontal;

        m_top.setNormal(farTopLeft, farTopRight, translation);
        m_bottom.setNormal(farBottomRight, farBottomLeft, translation);
        m_right.setNormal(farTopRight, farBottomRight, translation);
        m_left.setNormal(farBottomLeft, farTopLeft, translation);
    }
}