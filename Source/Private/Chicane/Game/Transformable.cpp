#include "Chicane/Game/Transformable.hpp"

namespace Chicane
{
    Transformable::Transformable()
        : m_position(Mat<4, float>(1.0f)),
        m_transform({}),
        m_transformObservable(std::make_unique<Observable<const Transform&>>())
    {}

    const Vec<3, float>& Transformable::getTranslation() const
    {
        return m_transform.translation;
    }

    void Transformable::setRelativeTranslation(const Vec<3, float>& inTranslation)
    {
        Vec<3, float> translation = m_transform.translation;
        translation.x += inTranslation.x;
        translation.y += inTranslation.y;
        translation.z += inTranslation.z;

        updateTranslation(translation);
    }

    void Transformable::setAbsoluteTranslation(const Vec<3, float>& inTranslation)
    {
        updateTranslation(inTranslation);
    }


    const Vec<3, float>& Transformable::getRotation() const
    {
        return m_transform.rotation;
    }

    void Transformable::setRelativeRotation(const Vec<3, float>& inRotation)
    {
        Vec<3, float> rotation = m_transform.rotation;
        rotation.x += inRotation.x;
        rotation.y += inRotation.y;
        rotation.z += inRotation.z;

        updateRotation(rotation);
    }

    void Transformable::setAbsoluteRotation(const Vec<3, float>& inRotation)
    {
        updateRotation(inRotation);
    }

    const Vec<3, float>& Transformable::getScale() const
    {
        return m_transform.scale;
    }

    void Transformable::setRelativeScale(const Vec<3, float>& inScale)
    {
        Vec<3, float> scale = m_transform.scale;
        scale.x += inScale.x;
        scale.y += inScale.y;
        scale.z += inScale.z;

        updateScale(scale);
    }

    void Transformable::setAbsoluteScale(const Vec<3, float>& inScale)
    {
        updateScale(inScale);
    }

    const Mat<4, float>& Transformable::getPosition() const
    {
        return m_position;
    }

    const Vec<3, float>& Transformable::getForward() const
    {
        return m_direction.forward;
    }

    const Vec<3, float>& Transformable::getRight() const
    {
        return m_direction.right;
    }

    const Vec<3, float>& Transformable::getUp() const
    {
        return m_direction.up;
    }

    void Transformable::watchTransform(
        std::function<void (const Transform&)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_transformObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
    }

    void Transformable::updateTranslation(const Vec<3, float>& inTranslation)
    {
        bool isIdentical = std::fabs(m_transform.translation.x - inTranslation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.y - inTranslation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.translation.z - inTranslation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_position = glm::translate(m_position, inTranslation);

        m_transform.translation = inTranslation;

        m_transformObservable->next(m_transform);
    }

    void Transformable::updateRotation(const Vec<3, float>& inRotation)
    {
        bool isIdentical = std::fabs(m_transform.rotation.x - inRotation.x) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.y - inRotation.y) < FLT_EPSILON &&
                           std::fabs(m_transform.rotation.z - inRotation.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        // Position
        m_position = glm::rotate(
            m_position,
            glm::radians(inRotation.x),
            FORWARD_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            glm::radians(inRotation.y),
            RIGHT_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            glm::radians(inRotation.z),
            UP_DIRECTION
        );

        // Orientation
        m_orientation = glm::angleAxis(
            glm::radians(m_transform.rotation.x),
            FORWARD_DIRECTION
        );
        m_orientation *= glm::angleAxis(
            glm::radians(m_transform.rotation.y),
            UP_DIRECTION
        );
        m_orientation *= glm::angleAxis(
            glm::radians(m_transform.rotation.z),
            RIGHT_DIRECTION
        );
        m_orientation = glm::normalize(m_orientation);

        m_direction.forward  = glm::rotate(m_orientation, FORWARD_DIRECTION);
        m_direction.right    = glm::rotate(m_orientation, RIGHT_DIRECTION);
        m_direction.up       = glm::rotate(m_orientation, UP_DIRECTION);

        m_transform.rotation = inRotation;

        m_transformObservable->next(m_transform);
    }

    void Transformable::updateScale(const Vec<3, float>& inScale)
    {
        bool isIdentical = std::fabs(m_transform.scale.x - inScale.x) < FLT_EPSILON &&
                           std::fabs(m_transform.scale.y - inScale.y) < FLT_EPSILON &&
                           std::fabs(m_transform.scale.z - inScale.z) < FLT_EPSILON;

        if (isIdentical)
        {
            return;
        }

        m_position = glm::scale(m_position, inScale);

        m_transform.scale = inScale;

        m_transformObservable->next(m_transform);
    }
}