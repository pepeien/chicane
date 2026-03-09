#include "Chicane/Core/Math/Transform/Spatial.reflected.hpp"

namespace Chicane
{
    SpatialTransform::SpatialTransform()
        : m_relative({}),
          m_absolute({})
    {}

    const Vec3& SpatialTransform::getRelativeTranslation() const
    {
        return m_relative.getTranslation();
    }

    const Rotator& SpatialTransform::getRelativeRotation() const
    {
        return m_relative.getRotation();
    }

    const Vec3& SpatialTransform::getRelativeScale() const
    {
        return m_relative.getScale();
    }

    const Vec3& SpatialTransform::getAbsoluteTranslation() const
    {
        return m_absolute.getTranslation();
    }

    const Rotator& SpatialTransform::getAbsoluteRotation() const
    {
        return m_absolute.getRotation();
    }

    const Vec3& SpatialTransform::getAbsoluteScale() const
    {
        return m_absolute.getScale();
    }

    void SpatialTransform::lookAt(const Vec3& inTarget)
    {
        m_absolute.lookAt(inTarget);

        refresh();
    }

    void SpatialTransform::refresh()
    {
        const QuatFloat& absoluteRotation = m_absolute.getRotation().get();
        const QuatFloat& relativeRotation = m_relative.getRotation().get();
        const QuatFloat  worldRot         = absoluteRotation * relativeRotation;

        Vec3 localOffset   = m_relative.getTranslation() * m_absolute.getScale();
        Vec3 rotatedOffset = absoluteRotation * static_cast<glm::vec3>(localOffset);

        rotation.set(worldRot);
        translation = m_absolute.getTranslation() + rotatedOffset;
        scale       = m_absolute.getScale() * m_relative.getScale();

        Transform::refresh();

        onRefresh();

        emmitChanges();
    }
}