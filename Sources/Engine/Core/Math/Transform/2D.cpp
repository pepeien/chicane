#include "Chicane/Core/Math/Transform/2D.reflected.hpp"

#include <cmath>

namespace Chicane
{
    Transform2D::Transform2D()
        : Changeable(),
          translation(Vec2::Zero()),
          rotation(0.0f),
          scale(Vec2::One()),
          m_matrix(Mat3::One)
    {}

    const Mat3& Transform2D::getMatrix() const
    {
        return m_matrix;
    }

    void Transform2D::setTransform(const Transform2D& inTransform)
    {
        translation = inTransform.translation;
        rotation    = inTransform.rotation;
        scale       = inTransform.scale;

        refresh();
    }

    const Vec2& Transform2D::getTranslation() const
    {
        return translation;
    }

    void Transform2D::addTranslation(float inValue)
    {
        addTranslation(Vec2(inValue));
    }

    void Transform2D::addTranslation(float inX, float inY)
    {
        addTranslation(Vec2(inX, inY));
    }

    void Transform2D::addTranslation(const Vec2& inValue)
    {
        setTranslation(translation + inValue);
    }

    void Transform2D::setTranslation(float inValue)
    {
        setTranslation(Vec2(inValue));
    }

    void Transform2D::setTranslation(float inX, float inY)
    {
        setTranslation(Vec2(inX, inY));
    }

    void Transform2D::setTranslation(const Vec2& inValue)
    {
        translation = inValue;

        refresh();
    }

    float Transform2D::getRotation() const
    {
        return rotation;
    }

    void Transform2D::addRotation(float inDegrees)
    {
        setRotation(rotation + inDegrees);
    }

    void Transform2D::setRotation(float inDegrees)
    {
        rotation = inDegrees;

        refresh();
    }

    const Vec2& Transform2D::getScale() const
    {
        return scale;
    }

    void Transform2D::addScale(float inValue)
    {
        addScale(Vec2(inValue));
    }

    void Transform2D::addScale(float inX, float inY)
    {
        addScale(Vec2(inX, inY));
    }

    void Transform2D::addScale(const Vec2& inValue)
    {
        setScale(scale + inValue);
    }

    void Transform2D::setScale(float inValue)
    {
        setScale(Vec2(inValue));
    }

    void Transform2D::setScale(float inX, float inY)
    {
        setScale(Vec2(inX, inY));
    }

    void Transform2D::setScale(const Vec2& inValue)
    {
        scale = inValue;

        refresh();
    }

    void Transform2D::lookAt(const Vec2& inTarget)
    {
        const Vec2 delta = inTarget - translation;

        rotation = glm::degrees(std::atan2(delta.y, delta.x));

        refresh();
    }

    void Transform2D::refresh()
    {
        const float radians = glm::radians(rotation);
        const float cosine  = glm::cos(radians);
        const float sine    = glm::sin(radians);

        m_matrix = Mat3(
            cosine * scale.x,
            sine * scale.x,
            0.0f,
            -sine * scale.y,
            cosine * scale.y,
            0.0f,
            translation.x,
            translation.y,
            1.0f
        );
    }
}
