#include "Chicane/Core/Math/Transform.reflected.hpp"

namespace Chicane
{
    Transform::Transform()
        : Changeable(),
          translation(Vec3::sZero()),
          rotation({}),
          scale(Vec3::sOne()),
          m_matrix(Mat4::One)
    {}

    const Mat4& Transform::getMatrix() const
    {
        return m_matrix;
    }

    bool Transform::toPosition(
        const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport, Vec2& outPosition
    ) const
    {
        return m_matrix.toPosition(inView, inProjection, inViewport, outPosition);
    }

    Vec2 Transform::toPosition(const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport) const
    {
        return m_matrix.toPosition(inView, inProjection, inViewport);
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
        m_matrix = rotation.get().toMatrix();

        m_matrix[0][0] *= scale.x;
        m_matrix[0][1] *= scale.x;
        m_matrix[0][2] *= scale.x;

        m_matrix[1][0] *= scale.y;
        m_matrix[1][1] *= scale.y;
        m_matrix[1][2] *= scale.y;

        m_matrix[2][0] *= scale.z;
        m_matrix[2][1] *= scale.z;
        m_matrix[2][2] *= scale.z;

        m_matrix[3][0] = translation.x;
        m_matrix[3][1] = translation.y;
        m_matrix[3][2] = translation.z;
    }
}