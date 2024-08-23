#include "Chicane/Game/Actor.hpp"

#include "Chicane/Game/Components/Actor.hpp"
#include "Chicane/Game/Components/Camera.hpp"

namespace Chicane
{
    Actor::Actor()
        : m_bCanTick(false),
        m_position(Math<float, 4>::Mat(1.0f)),
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

    const Math<float, 4>::Mat& Actor::getPosition() const
    {
        return m_position;
    }

    void Actor::setRelativeTranslation(const Math<float, 3>::Vec& inTranslation)
    {
        Math<float, 3>::Vec translation = m_transform.translation;
        translation.x += inTranslation.x;
        translation.y += inTranslation.y;
        translation.z += inTranslation.z;

        updateTranslation(translation);
    }

    void Actor::setAbsoluteTranslation(const Math<float, 3>::Vec& inTranslation)
    {
        updateTranslation(inTranslation);
    }

    void Actor::setRelativeRotation(const Math<float, 3>::Vec& inRotation)
    {
        Math<float, 3>::Vec rotation = m_transform.rotation;
        rotation.x += inRotation.x;
        rotation.y += inRotation.y;
        rotation.z += inRotation.z;

        updateRotation(rotation);
    }

    void Actor::setAbsoluteRotation(const Math<float, 3>::Vec& inRotation)
    {
        updateRotation(inRotation);
    }

    void Actor::setRelativeScale(const Math<float, 3>::Vec& inScale)
    {
        Math<float, 3>::Vec scale = m_transform.scale;
        scale.x += inScale.x;
        scale.y += inScale.y;
        scale.z += inScale.z;

        updateScale(scale);
    }

    void Actor::setAbsoluteScale(const Math<float, 3>::Vec& inScale)
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

    void Actor::updateTranslation(const Math<float, 3>::Vec& inTranslation)
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

    void Actor::updateRotation(const Math<float, 3>::Vec& inRotation)
    {
        bool didChange = inRotation.x != m_transform.rotation.x ||
                         inRotation.y != m_transform.rotation.y ||
                         inRotation.z != m_transform.rotation.z;

        if (!didChange)
        {
            return;
        }

        Math<float, 3>::Vec radianAngles = glm::radians(inRotation);

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

    void Actor::updateScale(const Math<float, 3>::Vec& inScale)
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