#include "Chicane/Game/Transformable/Component/CollisionComponent.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    CollisionComponent::CollisionComponent()
        : Component()
    {
        setCanTick(true);
    }

    void CollisionComponent::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        Actor* attachment = getAttachment();

        for (Actor* actor : Application::getLevel()->getActors())
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
        return isAttached() && Application::hasLevel();
    }
}