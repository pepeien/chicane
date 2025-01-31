#include "Chicane/Game/Transformable/Actor/Instance.hpp"

#include "Chicane/Application.hpp"

static constexpr float FORCE_MAX_VELOCITY               = 0.3f;
static constexpr float FORCE_DEACCELERATION_COEFFICIENT = 0.0015f;

namespace Chicane
{
    Actor::Actor()
        : Transformable(),
        m_bCanCollide(false),
        m_bCanTick(false),
        m_bIsApplyingForce(false),
        m_forceDirection(Vec3Zero),
        m_forceVelocity(Vec3Zero)
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
        processCollision();
        processForce();

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

    bool Actor::isCollidingWith(const Vec<3, float>& inPoint) const
    {
        return getBounds().contains(inPoint);
    }

    void Actor::collideWith(const Actor* inSubject)
    {
        if (!canCollide())
        {
            return;
        }

        onCollision(inSubject);
    }

    void Actor::addForce(const Vec<3, float>& inDirection, float inForce)
    {
        if (m_bIsApplyingForce)
        {
            return;
        }

        m_bIsApplyingForce = true;
        m_forceDirection   = inDirection;
        m_forceVelocity    = m_forceDirection * inForce;
    }

    void Actor::processForce()
    {
        if (!m_bIsApplyingForce)
        {
            return;
        }

        setAbsoluteTranslation(getTranslation() + m_forceVelocity);

        m_forceVelocity -= m_forceDirection * FORCE_DEACCELERATION_COEFFICIENT;
        m_forceVelocity.x = std::max(
            m_forceVelocity.x,
            -FORCE_MAX_VELOCITY
        );
        m_forceVelocity.x = std::max(
            m_forceVelocity.y,
            -FORCE_MAX_VELOCITY
        );
        m_forceVelocity.x = std::max(
            m_forceVelocity.z,
            -FORCE_MAX_VELOCITY
        );
    }

    void Actor::resetForce()
    {
        if (!m_bIsApplyingForce)
        {
            return;
        }

        m_bIsApplyingForce = false;

        m_forceDirection.x = 0.0f;
        m_forceDirection.y = 0.0f;
        m_forceDirection.z = 0.0f;

        m_forceVelocity.x = 0.0f;
        m_forceVelocity.y = 0.0f;
        m_forceVelocity.z = 0.0f;
    }

    void Actor::processCollision()
    {
        if (!canCollide() || !Application::hasLevel())
        {
            return;
        }

        for (const Actor* actor : Application::getLevel()->getActors())
        {
            if (actor == this || !actor->canCollide() || !actor->isCollidingWith(this))
            {
                continue;
            }

            resetForce();

            setAbsoluteTranslation(getTranslation() + (actor->getTop() * 1.0f));
        }
    }
}