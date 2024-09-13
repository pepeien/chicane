#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : Transformable(),
        m_canTick(false)
    {}

    bool Actor::canCollide() const
    {
        return m_canCollide;
    }

    void Actor::setCanCollide(bool inCanCollide)
    {
        m_canCollide = inCanCollide;
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
        return m_canTick;
    }

    void Actor::setCanTick(bool inCanTick)
    {
        m_canTick = inCanTick;
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