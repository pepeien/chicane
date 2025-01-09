#include "Chicane/Game/Transformable/Instance.hpp"

#include "Chicane/Core.hpp"

static Chicane::Mat<4, float> BASE_MAT(1.0f);

namespace Chicane
{
    Transformable::Transformable()
        : m_transform({}),
        m_position(BASE_MAT),
        m_direction({}),
        m_baseBounds(),
        m_currentBounds(),
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

        setTranslation(m_position, translation);
    }

    void Transformable::setAbsoluteTranslation(const Vec<3, float>& inTranslation)
    {
        setTranslation(BASE_MAT, inTranslation);
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

        setRotation(m_position, rotation);
    }

    void Transformable::setAbsoluteRotation(const Vec<3, float>& inRotation)
    {
        setRotation(BASE_MAT, inRotation);
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

        setScale(m_position, scale);
    }

    void Transformable::setAbsoluteScale(const Vec<3, float>& inScale)
    {
        setScale(BASE_MAT, inScale);
    }

    const Transform& Transformable::getTransform() const
    {
        return m_transform;
    }

    const Mat<4, float>& Transformable::getPosition() const
    {
        return m_position;
    }

    const Quat<float>& Transformable::getOrientation() const
    {
        return m_orientation;
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

    const Vec<3, float>& Transformable::getTop() const
    {
        return m_currentBounds.top;
    }

    const Vec<3, float>& Transformable::getCenter() const
    {
        return m_currentBounds.center;
    }

    const Vec<3, float>& Transformable::getOrigin() const
    {
        return m_currentBounds.origin;
    }

    const Vec<3, float>& Transformable::getExtent() const
    {
        return m_currentBounds.extent;
    }

    const Bounds& Transformable::getBounds() const
    {
        return m_currentBounds;
    }

    void Transformable::refreshBounds()
    {
        m_currentBounds.extent = m_baseBounds.extent * getScale();

        Vec<3, float> extent = m_currentBounds.extent * 0.5f;

        m_currentBounds.origin  = m_baseBounds.origin;
        m_currentBounds.origin += getTranslation();

        m_currentBounds.center    = m_currentBounds.origin;
        m_currentBounds.center.z += m_currentBounds.extent.z * 0.5f;

        m_currentBounds.top    = m_currentBounds.origin;
        m_currentBounds.top.z += m_currentBounds.extent.z;
    }

    void Transformable::setBounds(const Bounds& inBounds)
    {
        m_baseBounds = inBounds;

        refreshBounds();
    }

    Subscription<const Transform&>* Transformable::watchTransform(
        std::function<void (const Transform&)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        Subscription<const Transform&>* result = m_transformObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
        result->next(m_transform);

        return result;
    }

    void Transformable::setTranslation(const Mat<4, float>& inBase, const Vec<3, float>& inTranslation)
    {
        m_transform.translation = inTranslation;

        refreshPosition(inBase);

        m_transformObservable->next(m_transform);
    }

    void Transformable::setRotation(const Mat<4, float>& inBase, const Vec<3, float>& inRotation)
    {
        m_transform.rotation = inRotation;

        refreshPosition(inBase);

        m_transformObservable->next(m_transform);
    }

    void Transformable::setScale(const Mat<4, float>& inBase, const Vec<3, float>& inScale)
    {
        m_transform.scale = inScale;

        refreshPosition(inBase);

        m_transformObservable->next(m_transform);
    }

    void Transformable::refreshPosition(const Mat<4, float>& inBase)
    {
        // Transalate
        m_position = glm::translate(
            inBase,
            m_transform.translation
        );

        // Rotate
        m_position = glm::rotate(
            m_position,
            glm::radians(m_transform.rotation.x),
            RIGHT_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            glm::radians(m_transform.rotation.y),
            FORWARD_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            glm::radians(m_transform.rotation.z),
            UP_DIRECTION
        );

        // Scale
        m_position = glm::scale(
            m_position,
            m_transform.scale
        );

        refreshBounds();
        refreshOrientation();
    }

    void Transformable::refreshOrientation()
    {
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

        m_direction.forward = glm::rotate(m_orientation, FORWARD_DIRECTION);
        m_direction.right   = glm::rotate(m_orientation, RIGHT_DIRECTION);
        m_direction.up      = glm::rotate(m_orientation, UP_DIRECTION);
    }
}