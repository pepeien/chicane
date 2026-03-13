#include "Chicane/Core/Math/Transform.reflected.hpp"

namespace Chicane
{
    Transform::Transform()
        : Changeable(),
          translation(Vec3::Zero()),
          rotation({}),
          scale(Vec3::One()),
          m_matrix(Mat4::One)
    {}

    const Mat4& Transform::getMatrix() const
    {
        return m_matrix;
    }

    void Transform::setTransform(const Transform& inTransform)
    {
        translation = inTransform.translation;
        rotation    = inTransform.rotation;
        scale       = inTransform.scale;

        refresh();
    }

    const Vec3& Transform::getTranslation() const
    {
        return translation;
    }

    void Transform::addTranslation(float inValue)
    {
        addTranslation(Vec3(inValue));
    }

    void Transform::addTranslation(float inX, float inY, float inZ)
    {
        addTranslation(Vec3(inX, inY, inZ));
    }

    void Transform::addTranslation(const Vec3& inTranslation)
    {
        setTranslation(translation + inTranslation);
    }

    void Transform::setTranslation(float inValue)
    {
        setTranslation(Vec3(inValue));
    }

    void Transform::setTranslation(float inX, float inY, float inZ)
    {
        setTranslation(Vec3(inX, inY, inZ));
    }

    void Transform::setTranslation(const Vec3& inTranslation)
    {
        translation = inTranslation;

        refresh();
    }

    const Vec3& Transform::getForward() const
    {
        return rotation.getForward();
    }

    const Vec3& Transform::getRight() const
    {
        return rotation.getRight();
    }

    const Vec3& Transform::getUp() const
    {
        return rotation.getUp();
    }

    const Rotator& Transform::getRotation() const
    {
        return rotation;
    }

    void Transform::lookAt(const Vec3& inTarget)
    {
        rotation.lookAt(getTranslation(), inTarget);

        refresh();
    }

    const Vec3& Transform::getScale() const
    {
        return scale;
    }

    void Transform::addScale(float inValue)
    {
        addScale(Vec3(inValue));
    }

    void Transform::addScale(float inX, float inY, float inZ)
    {
        addScale(Vec3(inX, inY, inZ));
    }

    void Transform::addScale(const Vec3& inScale)
    {
        setScale(scale + inScale);
    }

    void Transform::setScale(float inValue)
    {
        setScale(Vec3(inValue));
    }

    void Transform::setScale(float inX, float inY, float inZ)
    {
        setScale(Vec3(inX, inY, inZ));
    }

    void Transform::setScale(const Vec3& inScale)
    {
        scale = inScale;

        refresh();
    }

    void Transform::refresh()
    {
        m_matrix = glm::translate(Mat4::One, static_cast<glm::vec3>(translation));
        m_matrix *= glm::toMat4(rotation.get());
        m_matrix = glm::scale(m_matrix, static_cast<glm::vec3>(scale));
    }
}