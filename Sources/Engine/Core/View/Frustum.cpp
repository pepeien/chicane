#include "Chicane/Core/View/Frustum.hpp"

namespace Chicane
{
    bool ViewFrustum::contains(const Transformable* inSubject) const
    {
        const Bounds3D& bounds = inSubject->getBounds();
        const Vec3&     min    = bounds.getMin().transformed;
        const Vec3&     max    = bounds.getMax().transformed;

        return contains(min, max);
    }

    bool ViewFrustum::contains(const Vec3& inMin, const Vec3& inMax) const
    {
        return m_top.contains(inMin, inMax) && m_bottom.contains(inMin, inMax) && m_near.contains(inMin, inMax) &&
               m_far.contains(inMin, inMax) && m_right.contains(inMin, inMax) && m_left.contains(inMin, inMax);
    }

    void ViewFrustum::update(const Transformable* inView, const ViewSettings& inSettings)
    {
        const Vec3& translation = inView->getTranslation();

        const Vec3 up      = inView->getUp().normalize();
        const Vec3 right   = inView->getRight().normalize();
        const Vec3 forward = inView->getForward().normalize();

        const float aspectRatio = inSettings.aspectRatio;
        const float fieldOfView = glm::radians(inSettings.fieldOfView);
        const float nearClip    = inSettings.nearClip;
        const float farClip     = inSettings.farClip;

        const float halfVertical   = farClip * tanf(fieldOfView * 0.5f);
        const float halfHorizontal = halfVertical * aspectRatio;

        const Vec3 nearCenter = translation + forward * nearClip;
        const Vec3 farCenter  = translation + forward * farClip;

        m_near.setNormal(forward, nearCenter);
        m_far.setNormal(forward * -1.0f, farCenter);

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