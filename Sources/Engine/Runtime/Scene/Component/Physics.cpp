#include "Chicane/Runtime/Scene/Component/Physics.hpp"

#include "Chicane/Runtime/Application.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

static inline constexpr const float FORCE_MAX_VELOCITY               = 0.3f;
static inline constexpr const float FORCE_DEACCELERATION_COEFFICIENT = 0.0009f;

namespace Chicane
{
    CPhysics::CPhysics()
        : Component(),
          m_bIsApplyingForce(false),
          m_forceDirection(Vec3::Zero),
          m_forceVelocity(Vec3::Zero)
    {
        setCanTick(true);
    }

    void CPhysics::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        updateCollision();
        updateForce();
    }

    bool CPhysics::canCollide() const
    {
        return isAttached() && Application::hasScene();
    }

    void CPhysics::addForce(const Vec3& inDirection, float inForce)
    {
        if (!canCollide() || m_bIsApplyingForce)
        {
            return;
        }

        m_bIsApplyingForce = true;
        m_forceDirection   = inDirection;
        m_forceVelocity    = m_forceDirection * inForce;
    }

    void CPhysics::updateCollision()
    {
        if (!canCollide())
        {
            return;
        }

        Actor* attachment = getParent<Actor>();

        if (!attachment)
        {
            return;
        }

        for (Actor* actor : Application::getScene()->getActors())
        {
            if (actor == attachment || !actor->canCollide() || !actor->isCollidingWith(attachment))
            {
                continue;
            }

            resetCollision(actor);

            attachment->collideWith(actor);
            actor->collideWith(attachment);
        }
    }

    void CPhysics::resetCollision(Actor* inSubject)
    {
        if (!canCollide())
        {
            return;
        }

        resetForce();

        Vec3 overlap = inSubject->getBounds().getOverlap(m_parent->getBounds());

        m_parent->addAbsoluteTranslation(overlap * 1.1f);
    }

    void CPhysics::updateForce()
    {
        if (!canCollide() || !m_bIsApplyingForce)
        {
            return;
        }

        Actor* attachment = getParent<Actor>();

        if (!attachment)
        {
            return;
        }

        attachment->setAbsoluteTranslation(attachment->getTranslation() + m_forceVelocity);

        m_forceVelocity -= m_forceDirection * FORCE_DEACCELERATION_COEFFICIENT;
        m_forceVelocity.x = std::max(m_forceVelocity.x, -FORCE_MAX_VELOCITY);
        m_forceVelocity.y = std::max(m_forceVelocity.y, -FORCE_MAX_VELOCITY);
        m_forceVelocity.z = std::max(m_forceVelocity.z, -FORCE_MAX_VELOCITY);
    }

    void CPhysics::resetForce()
    {
        if (!canCollide() || !m_bIsApplyingForce)
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
}