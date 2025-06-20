#include "Chicane/Core/Math/Transform/Combined.hpp"

namespace Chicane
{
    const Mat4& TransformCombined::getMatrix() const
    {
        return m_matrix;
    }

    const Vec3& TransformCombined::getTranslation() const
    {
        return m_translation;
    }

    const Vec3& TransformCombined::getRotation() const
    {
        return m_rotation;
    }

    const Vec3& TransformCombined::getScale() const
    {
        return m_scale;
    }

    const Transform& TransformCombined::getRelativeTransform() const
    {
        return m_relative;
    }

    void TransformCombined::setRelativeTransform(const Transform& inTransform)
    {
        m_relative.setTransform(inTransform);

        refresh();
    }

    const Vec3& TransformCombined::getRelativeTranslation() const
    {
        return m_relative.getTranslation();
    }

    void TransformCombined::addRelativeTranslation(const Vec3& inTranslation)
    {
        m_relative.addRotation(inTranslation);

        refresh();
    }

    void TransformCombined::setRelativeTranslation(const Vec3& inTranslation)
    {
        m_relative.setTranslation(inTranslation);

        refresh();
    }

    const Vec3& TransformCombined::getRelativeRotation() const
    {
        return m_relative.getRotation();
    }

    void TransformCombined::addRelativeRotation(const Vec3& inRotation)
    {
        m_relative.addRotation(inRotation);

        refresh();
    }

    void TransformCombined::setRelativeRotation(const Vec3& inRotation)
    {
        m_relative.setRotation(inRotation);

        refresh();
    }

    const Vec3& TransformCombined::getRelativeScale() const
    {
        return m_relative.getScale();
    }

    void TransformCombined::addRelativeScale(const Vec3& inScale)
    {
        m_relative.addScale(inScale);

        refresh();
    }

    void TransformCombined::setRelativeScale(const Vec3& inScale)
    {
        m_relative.setScale(inScale);

        refresh();
    }

    const Transform& TransformCombined::getAbsoluteTransform() const
    {
        return m_absolute;
    }

    void TransformCombined::setAbsoluteTransform(const Transform& inTransform)
    {
        m_absolute.setTransform(inTransform);

        refresh();
    }

    const Vec3& TransformCombined::getAbsoluteTranslation() const
    {
        return m_absolute.getTranslation();
    }

    void TransformCombined::addAbsoluteTranslation(const Vec3& inTranslation)
    {
        m_absolute.addTranslation(inTranslation);

        refresh();
    }

    void TransformCombined::setAbsoluteTranslation(const Vec3& inTranslation)
    {
        m_absolute.setTranslation(inTranslation);

        refresh();
    }

    const Vec3& TransformCombined::getAbsoluteRotation() const
    {
        return m_absolute.getRotation();
    }

    void TransformCombined::addAbsoluteRotation(const Vec3& inRotation)
    {
        m_absolute.addRotation(inRotation);

        refresh();
    }

    void TransformCombined::setAbsoluteRotation(const Vec3& inRotation)
    {
        m_absolute.setRotation(inRotation);

        refresh();
    }

    const Vec3& TransformCombined::getAbsoluteScale() const
    {
        return m_absolute.getScale();
    }

    void TransformCombined::addAbsoluteScale(const Vec3& inScale)
    {
        m_absolute.addScale(inScale);

        refresh();
    }

    void TransformCombined::setAbsoluteScale(const Vec3& inScale)
    {
        m_absolute.setScale(inScale);

        refresh();
    }

    const Vec3& TransformCombined::getRight() const
    {
        return m_right;
    }

    const Vec3& TransformCombined::getForward() const
    {
        return m_forward;
    }

    const Vec3& TransformCombined::getUp() const
    {
        return m_up;
    }

    void TransformCombined::refresh()
    {
        // Transformation
        m_matrix = m_absolute.getMatrix() * m_relative.getMatrix();

        m_translation = m_absolute.getTranslation() + m_relative.getTranslation();
        m_rotation    = m_absolute.getRotation() + m_relative.getRotation();
        m_scale       = m_absolute.getScale() * m_relative.getScale();

        refreshOrientation(glm::quat_cast(m_matrix));
    }
}