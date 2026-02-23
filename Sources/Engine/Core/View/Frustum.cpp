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

        m_near.update(forward, nearCenter);
        m_far.update(-forward, farCenter);

        const Vec3 farTopRight    = farCenter + up * halfVertical + right * halfHorizontal;
        const Vec3 farTopLeft     = farCenter + up * halfVertical - right * halfHorizontal;
        const Vec3 farBottomRight = farCenter - up * halfVertical + right * halfHorizontal;
        const Vec3 farBottomLeft  = farCenter - up * halfVertical - right * halfHorizontal;

        const Vec3 camPos = translation;

        {
            Vec3 edge1  = farTopLeft - camPos;
            Vec3 edge2  = farTopRight - camPos;
            Vec3 normal = edge2.cross(edge1).normalize();
            m_top.update(normal, camPos);
        }

        {
            Vec3 edge1  = farBottomRight - camPos;
            Vec3 edge2  = farBottomLeft - camPos;
            Vec3 normal = edge2.cross(edge1).normalize();
            m_bottom.update(normal, camPos);
        }

        {
            Vec3 edge1  = farTopRight - camPos;
            Vec3 edge2  = farBottomRight - camPos;
            Vec3 normal = edge2.cross(edge1).normalize();
            m_right.update(normal, camPos);
        }

        {
            Vec3 edge1  = farBottomLeft - camPos;
            Vec3 edge2  = farTopLeft - camPos;
            Vec3 normal = edge2.cross(edge1).normalize();
            m_left.update(normal, camPos);
        }
    }
}