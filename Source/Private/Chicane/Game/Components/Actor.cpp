#include "Chicane/Game/Components/Actor.hpp"

namespace Chicane
{
    ActorComponent::ActorComponent()
    :   m_owner(nullptr)
    {}

    bool ActorComponent::hasOwner() const
    {
        return m_owner != nullptr;
    }

    void ActorComponent::setOwner(Actor* inActor) {
        m_owner = inActor;
    }
}