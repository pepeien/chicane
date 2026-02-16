#include "Chicane/Runtime/Scene/Actor.hpp"

#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    Actor::Actor()
        : Object(),
          m_bCanCollide(false)
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

        return isCollidingWith(inSubject->getTranslation());
    }

    bool Actor::isCollidingWith(const Bounds3D& inBounds) const
    {
        return getBounds().intersects(inBounds);
    }

    bool Actor::isCollidingWith(const Vec3& inPoint) const
    {
        return getBounds().intersects(inPoint);
    }

    void Actor::collideWith(const Actor* inSubject)
    {
        if (!canCollide() || !inSubject)
        {
            return;
        }

        onCollision(inSubject);
    }
}