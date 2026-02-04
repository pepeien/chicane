#include "Chicane/Core/Math/Transform/Spatial.hpp"

namespace Chicane
{
    const Mat4& SpatialTransform::getMatrix() const
    {
        return m_matrix;
    }

    const Vec3& SpatialTransform::getTranslation() const
    {
        return m_translation;
    }

    const Rotator& SpatialTransform::getRotation() const
    {
        return m_rotation;
    }

    const Vec3& SpatialTransform::getScale() const
    {
        return m_scale;
    }

    const Vec3& SpatialTransform::getRelativeTranslation() const
    {
        return m_relative.getTranslation();
    }

    void SpatialTransform::addRelativeTranslation(const Vec3& inTranslation)
    {
        m_relative.addTranslation(inTranslation);

        refresh();
    }

    void SpatialTransform::setRelativeTranslation(const Vec3& inTranslation)
    {
        m_relative.setTranslation(inTranslation);

        refresh();
    }

    const Rotator& SpatialTransform::getRelativeRotation() const
    {
        return m_relative.getRotation();
    }

    void SpatialTransform::addRelativeRotation(const Rotator& inRotation)
    {
        m_relative.addLocalRotation(inRotation);

        refresh();
    }

    void SpatialTransform::setRelativeRotation(const Rotator& inRotation)
    {
        m_relative.setRotation(inRotation);

        refresh();
    }

    const Vec3& SpatialTransform::getRelativeScale() const
    {
        return m_relative.getScale();
    }

    void SpatialTransform::addRelativeScale(const Vec3& inScale)
    {
        m_relative.addScale(inScale);

        refresh();
    }

    void SpatialTransform::setRelativeScale(const Vec3& inScale)
    {
        m_relative.setScale(inScale);

        refresh();
    }

    const Vec3& SpatialTransform::getAbsoluteTranslation() const
    {
        return m_absolute.getTranslation();
    }

    void SpatialTransform::addAbsoluteTranslation(const Vec3& inTranslation)
    {
        m_absolute.addTranslation(inTranslation);

        refresh();
    }

    void SpatialTransform::setAbsoluteTranslation(const Vec3& inTranslation)
    {
        m_absolute.setTranslation(inTranslation);

        refresh();
    }

    const Rotator& SpatialTransform::getAbsoluteRotation() const
    {
        return m_absolute.getRotation();
    }

    void SpatialTransform::addAbsoluteRotation(const Rotator& inRotation)
    {
        m_absolute.addWorldRotation(inRotation);

        refresh();
    }

    void SpatialTransform::setAbsoluteRotation(const Rotator& inRotation)
    {
        m_absolute.setRotation(inRotation);

        refresh();
    }

    const Vec3& SpatialTransform::getAbsoluteScale() const
    {
        return m_absolute.getScale();
    }

    void SpatialTransform::addAbsoluteScale(const Vec3& inScale)
    {
        m_absolute.addScale(inScale);

        refresh();
    }

    void SpatialTransform::setAbsoluteScale(const Vec3& inScale)
    {
        m_absolute.setScale(inScale);

        refresh();
    }

    void SpatialTransform::refresh()
    {
        const QuatFloat& absoluteRotation = m_absolute.getRotation().get();
        const QuatFloat& relativeRotation = m_relative.getRotation().get();
        const QuatFloat  worldRot         = absoluteRotation * relativeRotation;

        Vec3 localOffset   = m_relative.getTranslation() * m_absolute.getScale();
        Vec3 rotatedOffset = absoluteRotation * localOffset;

        m_rotation.set(worldRot);
        m_translation = m_absolute.getTranslation() + rotatedOffset;
        m_scale       = m_absolute.getScale() * m_relative.getScale();

        Transform::refresh();
    }
}