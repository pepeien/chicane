#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : Transformable(),
        m_bCanTick(false)
    {}

    bool Actor::canCollide() const
    {
        return m_bCanCollide;
    }

    void Actor::setCanCollide(bool inCanCollide)
    {
        m_bCanCollide = inCanCollide;
    }

    void Actor::collide(Actor* inSubject)
    {
        if (!canCollide())
        {
            return;
        }

        onCollision(inSubject);
    }

    bool Actor::canTick() const
    {
        return m_bCanTick;
    }

    void Actor::setCanTick(bool inCanTick)
    {
        m_bCanTick = inCanTick;
    }

    void Actor::tick(float inDeltaTime)
    {
        if (!canTick())
        {
            return;
        }

        onTick(inDeltaTime);
    }
}