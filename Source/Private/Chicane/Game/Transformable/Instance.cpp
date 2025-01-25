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
        m_transformObservable(std::make_unique<Observable<void*>>())
    {}

    const Vec<3, float>& Transformable::getTranslation() const
    {
        return m_transform.translation;
    }

    const Vec<3, float>& Transformable::getRelativeTranslation() const
    {
        return m_transform.relative.translation;
    }

    void Transformable::addRelativeTranslation(float inTranslation)
    {
        addRelativeTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::addRelativeTranslation(float inX, float inY, float inZ)
    {
        addRelativeTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addRelativeTranslation(const Vec<3, float>& inTranslation)
    {
        setRelativeTranslation(m_transform.relative.translation + inTranslation);
    }

    void Transformable::setRelativeTranslation(float inTranslation)
    {
        setRelativeTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::setRelativeTranslation(float inX, float inY, float inZ)
    {
        setRelativeTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setRelativeTranslation(const Vec<3, float>& inTranslation)
    {
        setTranslation(inTranslation, m_transform.relative.translation);
    }

    const Vec<3, float>& Transformable::getWorldTranslation() const
    {
        return m_transform.world.translation;
    }

    void Transformable::addWorldTranslation(float inTranslation)
    {
        addWorldTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::addWorldTranslation(float inX, float inY, float inZ)
    {
        addWorldTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addWorldTranslation(const Vec<3, float>& inTranslation)
    {
        setWorldTranslation(m_transform.world.translation + inTranslation);
    }

    void Transformable::setWorldTranslation(float inTranslation)
    {
        setWorldTranslation(Vec<3, float>(inTranslation));
    }

    void Transformable::setWorldTranslation(float inX, float inY, float inZ)
    {
        setWorldTranslation(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setWorldTranslation(const Vec<3, float>& inTranslation)
    {
        setTranslation(inTranslation, m_transform.world.translation);
    }

    const Vec<3, float>& Transformable::getRotation() const
    {
        return m_transform.rotation;
    }

    const Vec<3, float>& Transformable::getRelativeRotation() const
    {
        return m_transform.relative.rotation;
    }

    void Transformable::addRelativeRotation(float inRotation)
    {
        addRelativeRotation(Vec<3, float>(inRotation));
    }

    void Transformable::addRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        addRelativeRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::addRelativeRotation(const Vec<3, float>& inRotation)
    {
        setRelativeRotation(m_transform.relative.rotation + inRotation);
    }

    void Transformable::setRelativeRotation(float inRotation)
    {
        setRelativeRotation(Vec<3, float>(inRotation));
    }

    void Transformable::setRelativeRotation(float inPitch, float inRoll, float inYaw)
    {
        setRelativeRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::setRelativeRotation(const Vec<3, float>& inRotation)
    {
        setRotation(inRotation, m_transform.relative.rotation);
    }

    const Vec<3, float>& Transformable::getWorldRotation() const
    {
        return m_transform.world.rotation;
    }

    void Transformable::addWorldRotation(float inRotation)
    {
        addWorldRotation(Vec<3, float>(inRotation));
    }

    void Transformable::addWorldRotation(float inPitch, float inRoll, float inYaw)
    {
        addWorldRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::addWorldRotation(const Vec<3, float>& inRotation)
    {
        setWorldRotation(m_transform.world.rotation + inRotation);
    }

    void Transformable::setWorldRotation(float inRotation)
    {
        setWorldRotation(Vec<3, float>(inRotation));
    }

    void Transformable::setWorldRotation(float inPitch, float inRoll, float inYaw)
    {
        setWorldRotation(Vec<3, float>(inPitch, inRoll, inYaw));
    }

    void Transformable::setWorldRotation(const Vec<3, float>& inRotation)
    {
        setRotation(inRotation, m_transform.world.rotation);
    }

    const Vec<3, float>& Transformable::getScale() const
    {
        return m_transform.scale;
    }

    const Vec<3, float>& Transformable::getRelativeScale() const
    {
        return m_transform.relative.scale;
    }

    void Transformable::addRelativeScale(float inScale)
    {
        addRelativeScale(Vec<3, float>(inScale));
    }

    void Transformable::addRelativeScale(float inX, float inY, float inZ)
    {
        addRelativeScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addRelativeScale(const Vec<3, float>& inScale)
    {
        setRelativeScale(m_transform.relative.scale + inScale);
    }

    void Transformable::setRelativeScale(float inScale)
    {
        setRelativeScale(Vec<3, float>(inScale));
    }

    void Transformable::setRelativeScale(float inX, float inY, float inZ)
    {
        setRelativeScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setRelativeScale(const Vec<3, float>& inScale)
    {
        setScale(inScale, m_transform.relative.scale);
    }

    const Vec<3, float>& Transformable::getWorldScale() const
    {
        return m_transform.world.scale;
    }

    void Transformable::addWorldScale(float inScale)
    {
        addWorldScale(Vec<3, float>(inScale));
    }

    void Transformable::addWorldScale(float inX, float inY, float inZ)
    {
        addWorldScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::addWorldScale(const Vec<3, float>& inScale)
    {
        setWorldScale(m_transform.world.scale + inScale);
    }

    void Transformable::setWorldScale(float inScale)
    {
        setWorldScale(Vec<3, float>(inScale));
    }

    void Transformable::setWorldScale(float inX, float inY, float inZ)
    {
        setWorldScale(Vec<3, float>(inX, inY, inZ));
    }

    void Transformable::setWorldScale(const Vec<3, float>& inScale)
    {
        setScale(inScale, m_transform.world.scale);
    }

    const Transform::Combined& Transformable::getTransform() const
    {
        return m_transform;
    }

    const Transform::Instance& Transformable::getRelativeTransform() const
    {
        return m_transform.relative;
    }

    void Transformable::setRelativeTransform(const Transform::Instance& inTransform)
    {
        setRelativeTranslation(inTransform.translation);
        setRelativeRotation(inTransform.rotation);
        setRelativeScale(inTransform.scale);
    }

    const Transform::Instance& Transformable::getWorldTransform() const
    {
        return m_transform.world;
    }

    void Transformable::setWorldTransform(const Transform::Instance& inTransform)
    {
        setWorldTranslation(inTransform.translation);
        setWorldRotation(inTransform.rotation);
        setWorldScale(inTransform.scale);
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

    Subscription<void*>* Transformable::watchTransform(
        std::function<void (void*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        return m_transformObservable->subscribe(inNext, inError, inComplete);
    }

    void Transformable::setTranslation(const Vec<3, float>& inTranslation, Vec<3, float>& outTranslation)
    {
        outTranslation = inTranslation;

        refreshCoordinates();

        m_transformObservable->next(nullptr);
    }

    void Transformable::setRotation(const Vec<3, float>& inRotation, Vec<3, float>& outRotation)
    {
        outRotation = inRotation;

        if (std::abs(outRotation.x) > 360.0f)
        {
            outRotation.x = 0.0f;
        }

        if (std::abs(outRotation.y) > 360.0f)
        {
            outRotation.y = 0.0f;
        }

        if (std::abs(outRotation.z) > 360.0f)
        {
            outRotation.z = 0.0f;
        }

        refreshCoordinates();

        m_transformObservable->next(nullptr);
    }

    void Transformable::setScale(const Vec<3, float>& inScale, Vec<3, float>& outScale)
    {
        outScale = inScale;

        refreshCoordinates();

        m_transformObservable->next(nullptr);
    }

    void Transformable::refreshCoordinates()
    {
        // Translation
        m_transform.translation = m_transform.world.translation;
        m_transform.translation = m_transform.translation + (getRight() * m_transform.relative.translation.x);
        m_transform.translation = m_transform.translation + (getForward() * m_transform.relative.translation.y);
        m_transform.translation = m_transform.translation + (getUp() * m_transform.relative.translation.z);

        // Rotation
        m_transform.rotation = m_transform.world.rotation + m_transform.relative.rotation;

        // Scaling
        m_transform.scale = m_transform.world.scale * m_transform.relative.scale;

        // Orientation
        refreshOrientation();
        refreshDirections();

        // Position
        refreshPosition();

        // Bounds
        refreshBounds();
    }

    void Transformable::refreshOrientation()
    {
        Quat<float> pitch = glm::angleAxis(glm::radians(m_transform.rotation.x), RIGHT_DIRECTION);
        Quat<float> roll  = glm::angleAxis(glm::radians(m_transform.rotation.y), FORWARD_DIRECTION);
        Quat<float> yaw   = glm::angleAxis(glm::radians(m_transform.rotation.z), UP_DIRECTION);

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