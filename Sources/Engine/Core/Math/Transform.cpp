#include "Chicane/Core/Math/Transform.hpp"

static const Chicane::Mat4 BASE_MAT(1.0f);

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

    const Vec3& Transform::getRotation() const
    {
        return m_rotation.getVector();
    }

    void Transform::addRotation(const Vec3& inRotation)
    {
        m_rotation.add(inRotation);
    }

    void Transform::setRotation(const Vec3& inRotation)
    {
        m_rotation.set(inRotation);

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

    const Vec3& Transform::getRight() const
    {
        return m_rotation.getRight();
    }

    const Vec3& Transform::getForward() const
    {
        return m_rotation.getForward();
    }

    const Vec3& Transform::getUp() const
    {
        return m_rotation.getUp();
    }

    const QuatFloat& Transform::getOrientation() const
    {
        return m_rotation.getOrientation();
    }

    void Transform::refresh()
    {
        m_matrix = glm::translate(Mat4::One, m_translation);
        m_matrix *= glm::toMat4(m_rotation.getOrientation());
        m_matrix = glm::scale(m_matrix, m_scale);
    }

}