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

    const Vec3& SpatialTransform::getRotation() const
    {
        return m_rotation;
    }

    const Vec3& SpatialTransform::getScale() const
    {
        return m_scale;
    }

    const Transform& SpatialTransform::getRelativeTransform() const
    {
        return m_relative;
    }

    void SpatialTransform::addRelativeTransform(const Transform& inTransform)
    {
        addRelativeTranslation(inTransform.getTranslation());
        addRelativeRotation(inTransform.getRotation());
        addRelativeScale(inTransform.getScale());
    }

    void SpatialTransform::setRelativeTransform(const Transform& inTransform)
    {
        m_relative.setTransform(inTransform);

        refresh();
    }

    const Vec3& SpatialTransform::getRelativeTranslation() const
    {
        return m_relative.getTranslation();
    }

    void SpatialTransform::addRelativeTranslation(const Vec3& inTranslation)
    {
        m_relative.addRotation(inTranslation);

        refresh();
    }

    void SpatialTransform::setRelativeTranslation(const Vec3& inTranslation)
    {
        m_relative.setTranslation(inTranslation);

        refresh();
    }

    const Vec3& SpatialTransform::getRelativeRotation() const
    {
        return m_relative.getRotation();
    }

    void SpatialTransform::addRelativeRotation(const Vec3& inRotation)
    {
        m_relative.addRotation(inRotation);

        refresh();
    }

    void SpatialTransform::setRelativeRotation(const Vec3& inRotation)
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

    const Transform& SpatialTransform::getAbsoluteTransform() const
    {
        return m_absolute;
    }

    void SpatialTransform::addAbsoluteTransform(const Transform& inTransform)
    {
        addAbsoluteTranslation(inTransform.getTranslation());
        addAbsoluteRotation(inTransform.getRotation());
        addAbsoluteScale(inTransform.getScale());
    }

    void SpatialTransform::setAbsoluteTransform(const Transform& inTransform)
    {
        m_absolute.setTransform(inTransform);

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

    const Vec3& SpatialTransform::getAbsoluteRotation() const
    {
        return m_absolute.getRotation();
    }

    void SpatialTransform::addAbsoluteRotation(const Vec3& inRotation)
    {
        m_absolute.addRotation(inRotation);

        refresh();
    }

    void SpatialTransform::setAbsoluteRotation(const Vec3& inRotation)
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

    const Vec3& SpatialTransform::getRight() const
    {
        return m_right;
    }

    const Vec3& SpatialTransform::getForward() const
    {
        return m_forward;
    }

    const Vec3& SpatialTransform::getUp() const
    {
        return m_up;
    }

    void SpatialTransform::refresh()
    {
        // Transformation
        m_matrix = m_absolute.getMatrix() * m_relative.getMatrix();

        m_translation = m_absolute.getTranslation() + m_relative.getTranslation();
        m_rotation    = m_absolute.getRotation() + m_relative.getRotation();
        m_scale       = m_absolute.getScale() * m_relative.getScale();

        refreshOrientation(glm::quat_cast(m_matrix));
    }
}