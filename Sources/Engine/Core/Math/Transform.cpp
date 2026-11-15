#include "Chicane/Core/Math/Transform.reflected.hpp"

#include <cfloat>
#include <cmath>

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

    Transform2D Transform::toScreenSpace(
        const Mat4& inView, const Mat4& inProjection, const Vec2& inViewport, const Vec3& inOffset
    ) const
    {
        const Vec3 worldPosition = getTranslation() + inOffset;
        const Vec4 clip          = inProjection * inView * Vec4(worldPosition, 1.0f);

        if (std::fabs(clip.w) < FLT_EPSILON || clip.w <= 0.0f)
        {
            return {};
        }

        const float inverseW = 1.0f / clip.w;
        const Vec2  ndc(clip.x * inverseW, clip.y * inverseW);

        Vec2 screenPosition = Vec2::sZero();
        screenPosition.x    = (ndc.x * 0.5f + 0.5f) * inViewport.x;
        screenPosition.y    = (0.5f - ndc.y * 0.5f) * inViewport.y;

        Transform2D result = {};
        result.setTranslation(screenPosition);
        result.setScale(Vec2(scale.x * inverseW, scale.y * inverseW));
        result.setRotation(0.0f);

        return result;
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