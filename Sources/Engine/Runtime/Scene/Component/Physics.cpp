#include "Chicane/Runtime/Scene/Component/Physics.reflected.hpp"

#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    CPhysics::CPhysics()
        : Component(),
          m_bodyID(JPH::BodyID::cInvalidBodyID),
          m_bodySettings({})
    {}

    void CPhysics::onLoad()
    {
        setCanTick(true);

        if (!isAttached())
        {
            return;
        }

        m_bodySettings.bounds = m_parent->getBounds();

        m_bodyID = Kerb::Engine::getInstance().createBody(m_bodySettings);
    }

    void CPhysics::onTick(float inDeltaTime)
    {
        if (!canCollide())
        {
            return;
        }

        Transform transform = Kerb::Engine::getInstance().getBodyTransform(m_bodyID);

        m_parent->setAbsoluteTranslation(transform.getTranslation());
    }

    void CPhysics::onActivation()
    {
        Kerb::Engine::getInstance().activateBody(m_bodyID);
    }

    void CPhysics::onDeactivation()
    {
        Kerb::Engine::getInstance().deactivateBody(m_bodyID);
    }

    void CPhysics::setShape(Kerb::BodyShape inType)
    {
        m_bodySettings.shape = inType;
    }

    void CPhysics::setShape(const Kerb::BodyPolygon& inPolygon)
    {
        m_bodySettings.shape   = Kerb::BodyShape::Polygon;
        m_bodySettings.polygon = std::move(inPolygon);
    }

    void CPhysics::setMotion(Kerb::MotionType inType)
    {
        m_bodySettings.motion = inType;
    }

    void CPhysics::moveTo(const Vec3& inLocation)
    {
        if (!canCollide())
        {
            return;
        }

        Transform transform;
        transform.setScale(m_parent->getScale());
        transform.setRotation(m_parent->getRotation());
        transform.setTranslation(inLocation);

        Kerb::Engine::getInstance().setBodyTransform(m_bodyID, transform);
    }

    void CPhysics::addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation)
    {
        if (!canCollide())
        {
            return;
        }

        Kerb::Engine::getInstance().addBodyImpulse(m_bodyID, inDirection, inForce, inLocation);
    }

    bool CPhysics::canCollide() const
    {
        return isAttached() && isActive() && hasBody();
    }

    bool CPhysics::hasBody() const
    {
        return !m_bodyID.IsInvalid();
    }
}