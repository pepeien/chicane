#include "Chicane/Core/Math/Transform.hpp"

namespace Chicane
{
    const Mat4& Transform::getMatrix() const
    {
        return m_matrix;
    }

    void Transform::setTransform(const Transform& inTransform)
    {
        m_translation = inTransform.m_translation;
        m_rotation    = inTransform.m_rotation;
        m_scale       = inTransform.m_scale;

        refresh();
    }

    const Vec3& Transform::getTranslation() const
    {
        return m_translation;
    }

    void Transform::addTranslation(const Vec3& inTranslation)
    {
        setTranslation(m_translation + inTranslation);
    }

    void Transform::setTranslation(const Vec3& inTranslation)
    {
        m_translation = inTranslation;

        refresh();
    }

    const Rotator& Transform::getRotation() const
    {
        return m_rotation;
    }

    void Transform::addLocalRotation(const Rotator& inRotation)
    {
        m_rotation.addLocal(inRotation.get());

        refresh();
    }

    void Transform::addWorldRotation(const Rotator& inRotation)
    {
        m_rotation.addWorld(inRotation.get());

        refresh();
    }

    void Transform::setRotation(const Rotator& inRotation)
    {
        m_rotation.set(inRotation.get());

        refresh();
    }

    const Vec3& Transform::getScale() const
    {
        return m_scale;
    }

    void Transform::addScale(const Vec3& inScale)
    {
        setScale(m_scale + inScale);
    }

    void Transform::setScale(const Vec3& inScale)
    {
        m_scale = inScale;

        refresh();
    }

    void Transform::refresh()
    {
        m_matrix = glm::translate(Mat4::One, m_translation);
        m_matrix *= glm::toMat4(m_rotation.get());
        m_matrix = glm::scale(m_matrix, m_scale);
    }
}