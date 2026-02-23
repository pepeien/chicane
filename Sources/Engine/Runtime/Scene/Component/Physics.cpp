#include "Chicane/Runtime/Scene/Component/Physics.hpp"

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    CPhysics::CPhysics()
        : Component(),
          m_bodyID(JPH::BodyID::cInvalidBodyID)
    {
        setCanTick(true);
    }

    void CPhysics::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        Transform transform = Kerb::Engine::getInstance().getTransform(m_bodyID);

        m_parent->setAbsoluteTranslation(transform.getTranslation());
    }

    void CPhysics::onAttachment(Object* inAttachment)
    {
        m_bodyID = bisDynamic ? Kerb::Engine::getInstance().createDynamicBody(
                                    inAttachment->getTranslation(),
                                    inAttachment->getRotation(),
                                    inAttachment->getBounds()
                                )
                              : Kerb::Engine::getInstance().createStaticBody(
                                    inAttachment->getTranslation(),
                                    inAttachment->getRotation(),
                                    inAttachment->getBounds()
                                );
    }

    bool CPhysics::canCollide() const
    {
        return isAttached() && !m_bodyID.IsInvalid();
    }

    void CPhysics::addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation)
    {
        if (!canCollide())
        {
            return;
        }

        Kerb::Engine::getInstance().addImpulse(m_bodyID, inDirection, inForce, inLocation);
    }
}