#include "Chicane/Runtime/Scene/Component/Physics.reflected.hpp"

#include <cmath>

#include "Chicane/Kerb/Engine.hpp"
#include "Chicane/Runtime/Scene.hpp"
#include "Chicane/Runtime/Scene/Actor.hpp"

namespace Chicane
{
    static Vec3 localBoundsSize(const Bounds3D& inBounds)
    {
        const Vec3& min = inBounds.getMin().base;
        const Vec3& max = inBounds.getMax().base;
        if (min.x >= max.x || min.y >= max.y || min.z >= max.z)
        {
            return Vec3::Zero();
        }

        return Vec3(max.x - min.x, max.y - min.y, max.z - min.z);
    }

    static bool changed(const Vec3& inLeft, const Vec3& inRight)
    {
        return std::abs(inLeft.x - inRight.x) > 0.01f || std::abs(inLeft.y - inRight.y) > 0.01f ||
               std::abs(inLeft.z - inRight.z) > 0.01f;
    }

    CPhysics::CPhysics()
        : Component(),
          m_body(Kerb::Body::invalid()),
          m_bodySettings({}),
          m_syncedScale(Vec3::One()),
          m_syncedLocalSize(Vec3::Zero()),
          m_actorToBody(Vec3::Zero()),
          m_bSyncingBody(false)
    {}

    CPhysics::~CPhysics()
    {
        destroyBody();
    }

    void CPhysics::onLoad()
    {
        ensureBody();
    }

    void CPhysics::onUnload()
    {
        destroyBody();
    }

    void CPhysics::onTick(float)
    {
        if (!canCollide())
        {
            return;
        }

        Kerb::Engine& physics = Kerb::Engine::getInstance();

        if (m_bodySettings.motion == Kerb::MotionType::Kinematic)
        {
            physics.setBodyTransform(m_body, makeActorTransform(m_parent->getAbsoluteTranslation()));

            return;
        }

        if (m_bodySettings.motion != Kerb::MotionType::Dynamic)
        {
            return;
        }

        const Transform transform = physics.getBodyTransform(m_body, true);
        const Vec3      location  = transform.getTranslation() - m_actorToBody;
        if (!changed(location, m_parent->getAbsoluteTranslation()))
        {
            return;
        }

        m_parent->setAbsoluteTranslation(location);
    }

    void CPhysics::onActivation()
    {
        ensureBody();

        if (!hasBody())
        {
            return;
        }

        Kerb::Engine::getInstance().activateBody(m_body);
    }

    void CPhysics::onDeactivation()
    {
        if (!hasBody())
        {
            return;
        }

        Kerb::Engine::getInstance().deactivateBody(m_body);
    }

    void CPhysics::onAttachment(Object* inParent)
    {
        Component::onAttachment(inParent);

        ensureBody();
    }

    void CPhysics::onRefresh()
    {
        Object::onRefresh();
        syncBody();
    }

    void CPhysics::setShape(Kerb::BodyShape inType)
    {
        m_bodySettings.shape = inType;
    }

    void CPhysics::setShape(const Kerb::BodyPolygon& inPolygon)
    {
        m_bodySettings.shape   = Kerb::BodyShape::Polygon;
        m_bodySettings.polygon = inPolygon;
    }

    void CPhysics::setMotion(Kerb::MotionType inType)
    {
        m_bodySettings.motion = inType;

        if (hasBody())
        {
            Kerb::Engine::getInstance().setBodyMotion(m_body, inType);
        }

        syncTickState();
    }

    void CPhysics::moveTo(const Vec3& inLocation)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        Kerb::Engine::getInstance().setBodyTransform(m_body, makeActorTransform(inLocation));
    }

    void CPhysics::moveBy(const Vec3& inOffset)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        const Vec3 current = Kerb::Engine::getInstance().getBodyTransform(m_body).getTranslation();
        moveTo(current + inOffset);
    }

    Vec3 CPhysics::getLinearVelocity() const
    {
        if (!hasBody())
        {
            return Vec3::Zero();
        }

        return Kerb::Engine::getInstance().getBodyLinearVelocity(m_body);
    }

    void CPhysics::setLinearVelocity(const Vec3& inVelocity)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        Kerb::Engine::getInstance().setBodyLinearVelocity(m_body, inVelocity);
    }

    void CPhysics::setHorizontalVelocity(const Vec3& inVelocity)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        Kerb::Engine::getInstance().setBodyHorizontalVelocity(m_body, inVelocity);
    }

    void CPhysics::addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation)
    {
        if (!canCollide())
        {
            return;
        }

        Kerb::Engine::getInstance().addBodyImpulse(m_body, inDirection, inForce, inLocation);
    }

    bool CPhysics::canCollide() const
    {
        return isAttached() && isActive() && hasBody();
    }

    bool CPhysics::hasBody() const
    {
        return m_body.isValid();
    }

    void CPhysics::ensureBody()
    {
        if (hasBody() || !isAttached())
        {
            return;
        }

        m_bodySettings.bounds = m_parent->getBounds();
        m_syncedScale         = m_parent->getAbsoluteScale();
        m_syncedLocalSize     = localBoundsSize(m_parent->getBounds());
        m_actorToBody         = m_parent->getBounds().getCenter() - m_parent->getAbsoluteTranslation();
        m_body                = Kerb::Engine::getInstance().createBody(m_bodySettings);

        if (!hasBody())
        {
            syncTickState();

            return;
        }

        if (isActive())
        {
            Kerb::Engine::getInstance().activateBody(m_body);
        }

        syncTickState();
    }

    void CPhysics::destroyBody()
    {
        if (!hasBody())
        {
            return;
        }

        Kerb::Engine::getInstance().destroyBody(m_body);
        m_body = Kerb::Body::invalid();
        syncTickState();
    }

    void CPhysics::syncBody()
    {
        if (m_bSyncingBody || !isAttached() || !hasBody() || !m_parent)
        {
            return;
        }

        const Vec3 scale     = m_parent->getAbsoluteScale();
        const Vec3 localSize = localBoundsSize(m_parent->getBounds());
        if (changed(scale, m_syncedScale) || changed(localSize, m_syncedLocalSize))
        {
            m_bSyncingBody = true;
            destroyBody();
            ensureBody();
            m_bSyncingBody = false;

            return;
        }

        if (m_bodySettings.motion == Kerb::MotionType::Static)
        {
            moveTo(m_parent->getBounds().getCenter());
        }
    }

    void CPhysics::syncTickState()
    {
        setCanTick(hasBody() && m_bodySettings.motion != Kerb::MotionType::Static);
    }

    Transform CPhysics::makeActorTransform(const Vec3& inLocation) const
    {
        Transform transform;
        if (!m_parent)
        {
            transform.setTranslation(inLocation);

            return transform;
        }

        transform.setScale(m_parent->getAbsoluteScale());
        transform.setRotation(m_parent->getAbsoluteRotation());
        transform.setTranslation(inLocation);

        return transform;
    }
}
