#include "Runtime/Game/Transformable/Component/Physics.hpp"

#include "Runtime/Application.hpp"
#include "Runtime/Game/Transformable/Actor.hpp"

static constexpr float FORCE_MAX_VELOCITY               = 0.3f;
static constexpr float FORCE_DEACCELERATION_COEFFICIENT = 0.0009f;

namespace Chicane
{
    CPhysics::CPhysics()
        : Component(),
        m_bIsApplyingForce(false),
        m_forceDirection(Vec3Zero),
        m_forceVelocity(Vec3Zero)
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
        return isAttached() && Application::hasLevel();
    }

    void CPhysics::addForce(const Vec<3, float>& inDirection, float inForce)
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

        Actor* attachment = getAttachment<Actor>();

        if (!attachment)
        {
            return;
        }

        for (Actor* actor : Application::getLevel()->getActors())
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

        Vec<3, float> overlap = inSubject->getBounds().getOverlap(m_attachment->getBounds());

        m_attachment->addAbsoluteTranslation(overlap * 1.1f);
    }

    void CPhysics::updateForce()
    {
        if (!canCollide() || !m_bIsApplyingForce)
        {
            return;
        }

        Actor* attachment = getAttachment<Actor>();

        if (!attachment)
        {
            return;
        }

        attachment->setAbsoluteTranslation(attachment->getTranslation() + m_forceVelocity);

        m_forceVelocity -= m_forceDirection * FORCE_DEACCELERATION_COEFFICIENT;
        m_forceVelocity.x = std::max(
            m_forceVelocity.x,
            -FORCE_MAX_VELOCITY
        );
        m_forceVelocity.y = std::max(
            m_forceVelocity.y,
            -FORCE_MAX_VELOCITY
        );
        m_forceVelocity.z = std::max(
            m_forceVelocity.z,
            -FORCE_MAX_VELOCITY
        );
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