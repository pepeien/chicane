#include "Chicane/Game/Transformable/Component/CollisionComponent.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    CollisionComponent::CollisionComponent()
        : Component()
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

    void CollisionComponent::onDeactivation()
    {
        if (!hasActiveLevel())
        {
            return;
        }

        removeComponent(this);
    }

    void CollisionComponent::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        Actor* attachment = getAttachment();

        for (Actor* actor : getActiveLevel()->getActors())
        {
            if (actor == attachment || !actor->canCollide())
            {
                continue;
            }

            if (!actor->isCollidingWith(attachment))
            {
                continue;
            }

            attachment->collideWith(actor);
            actor->collideWith(attachment);
        }
    }

    bool CollisionComponent::canCollide() const
    {
        return isAttached() && hasActiveLevel();
    }
}