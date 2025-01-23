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

    void Transformable::addTranslation(float inTranslation)
    {
        addTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::addTranslation(float inX, float inY, float inZ)
    {
        addTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addTranslation(const Vec<3, float>& inTranslation)
    {
        Vec<3, float> translation = m_transform.translation;
        translation.x += inTranslation.x;
        translation.y += inTranslation.y;
        translation.z += inTranslation.z;

        setTranslation(translation);
    }

    void Transformable::setTranslation(float inTranslation)
    {
        setTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::setTranslation(float inX, float inY, float inZ)
    {
        setTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setTranslation(const Vec<3, float>& inTranslation)
    {
        m_transform.translation = inTranslation;

        refreshCoordinates();

        m_transformObservable->next(m_transform);
    }

    const Vec<3, float>& Transformable::getRotation() const
    {
        return m_transform.rotation;
    }

    void Transformable::addYaw(float inDegrees)
    {
        addRotation(inDegrees, 0.0f, 0.0f);
    }

    void Transformable::setYaw(float inDegrees)
    {
        setRotation(inDegrees, 0.0f, 0.0f);
    }

    void Transformable::addRoll(float inDegrees)
    {
        addRotation(0.0f, inDegrees, 0.0f);
    }

    void Transformable::setRoll(float inDegrees)
    {
        setRotation(0.0f, inDegrees, 0.0f);
    }

    void Transformable::addPitch(float inDegrees)
    {
        addRotation(0.0f, 0.0f, inDegrees);
    }

    void Transformable::setPitch(float inDegrees)
    {
        setRotation(0.0f, 0.0f, inDegrees);
    }

    void Transformable::addRotation(float inRotation)
    {
        addRotation(Vec<3, float>(inRotation));
    }

    void Transformable::addRotation(float inYaw, float inRoll, float inPitch)
    {
        addRotation(Vec<3, float>(inYaw, inRoll, inPitch));
    }

    void Transformable::addRotation(const Vec<3, float>& inRotation)
    {
        Vec<3, float> rotation = m_transform.rotation;
        rotation.x += inRotation.x;
        rotation.y += inRotation.y;
        rotation.z += inRotation.z;

        setRotation(rotation);
    }

    void Transformable::setRotation(float inRotation)
    {
        setRotation(Vec<3, float>(inRotation));
    }

    void Transformable::setRotation(float inYaw, float inRoll, float inPitch)
    {
        setRotation(Vec<3, float>(inYaw, inRoll, inPitch));
    }

    void Transformable::setRotation(const Vec<3, float>& inRotation)
    {
        m_transform.rotation = inRotation;

        if (std::abs(m_transform.rotation.x) > 360.0f)
        {
            m_transform.rotation.x = 0.0f;
        }

        if (std::abs(m_transform.rotation.y) > 360.0f)
        {
            m_transform.rotation.y = 0.0f;
        }

        if (std::abs(m_transform.rotation.z) > 360.0f)
        {
            m_transform.rotation.z = 0.0f;
        }

        refreshCoordinates();

        m_transformObservable->next(m_transform);
    }

    const Vec<3, float>& Transformable::getScale() const
    {
        return m_transform.scale;
    }

    void Transformable::addScale(float inScale)
    {
        addScale(Vec<3, float>(inScale));
    }

    void Transformable::addScale(float inX, float inY, float inZ)
    {
        addScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addScale(const Vec<3, float>& inScale)
    {
        Vec<3, float> scale = m_transform.scale;
        scale.x += inScale.x;
        scale.y += inScale.y;
        scale.z += inScale.z;

        setScale(scale);
    }

    void Transformable::setScale(float inScale)
    {
        setScale(Vec<3, float>(inScale));
    }

    void Transformable::setScale(float inX, float inY, float inZ)
    {
        setScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setScale(const Vec<3, float>& inScale)
    {
        m_transform.scale = inScale;

        refreshCoordinates();

        m_transformObservable->next(m_transform);
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
        std::function<void (const Transform&)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        Subscription<const Transform&>* subscription = m_transformObservable->subscribe(
            inNext,
            inError,
            inComplete
        );
        subscription->next(m_transform);

        return subscription;
    }

    void Transformable::refreshCoordinates()
    {
        refreshOrientation();
        refreshDirections();
        refreshPosition();
        refreshBounds();
    }

    void Transformable::refreshOrientation()
    {
        Quat<float> pitch = glm::angleAxis(glm::radians(m_transform.rotation.z), RIGHT_DIRECTION);
        Quat<float> roll  = glm::angleAxis(glm::radians(m_transform.rotation.y), FORWARD_DIRECTION);
        Quat<float> yaw   = glm::angleAxis(glm::radians(m_transform.rotation.x), UP_DIRECTION);

        m_orientation = roll * yaw * pitch;
        m_orientation = glm::normalize(m_orientation);
    }

    void Transformable::refreshDirections()
    {
        m_direction.forward = glm::rotate(m_orientation, FORWARD_DIRECTION);
        m_direction.right   = glm::rotate(m_orientation, RIGHT_DIRECTION);
        m_direction.up      = glm::rotate(m_orientation, UP_DIRECTION);
    }

    void Transformable::refreshPosition()
    {
        m_position  = glm::translate(BASE_MAT, m_transform.translation); // Transalate
        m_position *= glm::toMat4(m_orientation);                        // Rotate
        m_position  = glm::scale(m_position, m_transform.scale);         // Scale
    }
}