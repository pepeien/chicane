#include "Chicane/Game/Actor/Component/Collision.hpp"

#include <iostream>

#include "Chicane/Core.hpp"

namespace Chicane
{
    CollisionComponent::CollisionComponent()
        : ActorComponent()
    {
        setCanTick(true);
    }

    void CollisionComponent::onActivation()
    {
        if (!hasActiveLevel())
        {
            return;
        }

        addComponent(this);
    }

    void CollisionComponent::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        Actor* attachment = getAttachment();

        const Bounds& bounds = attachment->getBounds();

        for (Actor* actor : getActiveLevel()->getActors())
        {
            if (actor == attachment || !actor->canCollide())
            {
                continue;
            }

            if (!bounds.contains(actor->getBounds()))
            {
                continue;
            }

            attachment->collide(actor);
            actor->collide(attachment);
        }
    }

    bool CollisionComponent::canCollide() const
    {
        return isAttached() && hasActiveLevel();
    }
}