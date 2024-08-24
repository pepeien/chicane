#include "Chicane/Game/Components/Actor.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    ActorComponent::ActorComponent()
        : m_canTick(false),
        m_owner(nullptr)
    {
        addComponent(this);
    }

    bool ActorComponent::isActive() const
    {
        return m_isActive;
    }

    void ActorComponent::activate()
    {
        m_isActive = true;

        onActivation();
    }

    void ActorComponent::deactivate()
    {
        m_isActive = false;

        onDeactivation();
    }

    bool ActorComponent::canTick() const
    {
        return m_isActive && m_canTick;
    }

    void ActorComponent::setCanTick(bool inCanTick)
    {
        m_canTick = inCanTick;
    }

    const Vec<3, float>& ActorComponent::getTranslation() const
    {
        return m_transform.translation;
    }

    void ActorComponent::setTranslation(const Vec<3, float>& inTranslation)
    {
        m_transform.translation = inTranslation;
    }

    const Vec<3, float>& ActorComponent::getRotation() const
    {
        return m_transform.rotation;
    }

    void ActorComponent::setRotation(const Vec<3, float>& inRotation)
    {
        m_transform.rotation = inRotation;
    }

    bool ActorComponent::hasOwner() const
    {
        return m_owner != nullptr;
    }

    void ActorComponent::setOwner(Actor* inActor) {
        m_owner = inActor;

        onAttachment();
    }
}