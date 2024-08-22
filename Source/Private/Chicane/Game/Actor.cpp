#include "Chicane/Game/Actor.hpp"

#include "Chicane/Game/Components/Actor.hpp"
#include "Chicane/Game/Components/Camera.hpp"

namespace Chicane
{
    Actor::Actor()
        : m_bCanTick(false),
        m_position(Mat<float>::Four(1.0f)),
        m_transform({}),
        m_camera(nullptr)
    {}

    Actor::~Actor()
    {
        delete m_camera;
    }

    bool Actor::canTick() const
    {
        return m_bCanTick;
    }

    void Actor::setCanTick(bool bInCanTick)
    {
        m_bCanTick = bInCanTick;
    }

    const Mat<float>::Four& Actor::getPosition() const
    {
        return m_position;
    }

    void Actor::setRelativeTranslation(const Vec<float>::Three& inTranslation)
    {
        Vec<float>::Three translation = m_transform.translation;
        translation.x += inTranslation.x;
        translation.y += inTranslation.y;
        translation.z += inTranslation.z;

        updateTranslation(translation);
    }

    void Actor::setAbsoluteTranslation(const Vec<float>::Three& inTranslation)
    {
        updateTranslation(inTranslation);
    }

    void Actor::setRelativeRotation(const Vec<float>::Three& inRotation)
    {
        Vec<float>::Three rotation = m_transform.rotation;
        rotation.x += inRotation.x;
        rotation.y += inRotation.y;
        rotation.z += inRotation.z;

        updateRotation(rotation);
    }

    void Actor::setAbsoluteRotation(const Vec<float>::Three& inRotation)
    {
        updateRotation(inRotation);
    }

    void Actor::setRelativeScale(const Vec<float>::Three& inScale)
    {
        Vec<float>::Three scale = m_transform.scale;
        scale.x += inScale.x;
        scale.y += inScale.y;
        scale.z += inScale.z;

        updateScale(scale);
    }

    void Actor::setAbsoluteScale(const Vec<float>::Three& inScale)
    {
        updateScale(inScale);
    }

    bool Actor::hasCamera() const
    {
        return m_camera != nullptr;
    }

    const std::vector<ActorComponent*>& Actor::getComponents() const
    {
        return m_components;
    }

    bool Actor::hasMesh() const
    {
        return !m_mesh.name.empty();
    }

    const Box::Instance& Actor::getMesh() const
    {
        return m_mesh;
    }

    void Actor::updateTranslation(const Vec<float>::Three& inTranslation)
    {
        bool didChange = inTranslation.x != m_transform.translation.x ||
                         inTranslation.y != m_transform.translation.y ||
                         inTranslation.z != m_transform.translation.z;

        if (!didChange)
        {
            return;
        }

        m_position = glm::translate(m_position, inTranslation);

        m_transform.translation = inTranslation;
    }

    void Actor::updateRotation(const Vec<float>::Three& inRotation)
    {
        bool didChange = inRotation.x != m_transform.rotation.x ||
                         inRotation.y != m_transform.rotation.y ||
                         inRotation.z != m_transform.rotation.z;

        if (!didChange)
        {
            return;
        }

        Vec<float>::Three radianAngles = glm::radians(inRotation);

        m_position = glm::rotate(
            m_position,
            radianAngles.x,
            FORWARD_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            radianAngles.y,
            RIGHT_DIRECTION
        );
        m_position = glm::rotate(
            m_position,
            radianAngles.z,
            UP_DIRECTION
        );

        m_transform.rotation = inRotation;
    }

    void Actor::updateScale(const Vec<float>::Three& inScale)
    {
        bool didChange = inScale.x != m_transform.scale.x ||
                         inScale.y != m_transform.scale.y ||
                         inScale.z != m_transform.scale.z;

        if (!didChange)
        {
            return;
        }

        m_position = glm::scale(m_position, inScale);

        m_transform.scale = inScale;
    }
}