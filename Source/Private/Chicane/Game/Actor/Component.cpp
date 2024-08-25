#include "Chicane/Game/Actor/Component.hpp"

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

    bool ActorComponent::hasOwner() const
    {
        return m_owner != nullptr;
    }

    void ActorComponent::setOwner(Actor* inActor) {
        m_owner = inActor;

        onAttachment();
    }
}