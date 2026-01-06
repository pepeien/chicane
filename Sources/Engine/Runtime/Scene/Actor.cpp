#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    Actor::Actor()
        : Transformable(),
        m_bCanCollide(false),
        m_bCanTick(false),
        m_id("")
    {}

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

        return isCollidingWith(inSubject->getTranslation());
    }

    bool Actor::isCollidingWith(const Bounds& inBounds) const
    {
        return getBounds().contains(inBounds);
    }

    bool Actor::isCollidingWith(const Vec3& inPoint) const
    {
        return getBounds().contains(inPoint);
    }

    void Actor::collideWith(const Actor* inSubject)
    {
        if (!canCollide() || !inSubject)
        {
            return;
        }

        onCollision(inSubject);
    }

    const String& Actor::getId() const
    {
        return m_id;
    }

    void Actor::setId(const String& inId)
    {
        m_id = inId;
    }
}