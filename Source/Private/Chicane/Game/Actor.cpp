#include "Chicane/Game/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : Transformable(),
        m_bCanCollide(false),
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

    bool Actor::isCollidingWith(const Actor* inSubject) const
    {
        if (!inSubject)
        {
            return false;
        }

        return isCollidingWith(inSubject->getBounds());
    }

    bool Actor::isCollidingWith(const Bounds& inBounds) const
    {
        return m_currentBounds.contains(inBounds);
    }

    bool Actor::isCollidingWith(const Vec<3, float>& inPoint) const
    {
        return m_currentBounds.contains(inPoint);
    }

    void Actor::collideWith(const Actor* inSubject)
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