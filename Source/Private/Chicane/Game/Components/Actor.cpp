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

    bool ActorComponent::canTick() const
    {
        return m_canTick;
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