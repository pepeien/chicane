#include "Chicane/Game/Transformable/Component/Collision.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

namespace Chicane
{
    CCollision::CCollision()
        : Component()
    {
        setCanTick(true);

        activate();
    }

    void CCollision::onTick(float inDeltaTime)
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

    bool CCollision::canCollide() const
    {
        return isAttached() && Application::hasLevel();
    }
}