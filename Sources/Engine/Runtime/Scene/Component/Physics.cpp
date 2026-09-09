#include "Chicane/Runtime/Scene/Component/Physics.reflected.hpp"

#include <cmath>

#include "Chicane/Kerb/Engine.hpp"
#include "Chicane/Kerb/Collision/Preset/Info.hpp"
#include "Chicane/Renderer/Debug.hpp"
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
            updateCollision();

            return;
        }

        if (m_bodySettings.motion != Kerb::MotionType::Dynamic)
        {
            return;
        }

        const Transform transform = physics.getBodyTransform(m_body, true);
        const Vec3      location  = transform.getTranslation() - m_actorToBody;
        if (changed(location, m_parent->getAbsoluteTranslation()))
        {
            m_parent->setAbsoluteTranslation(location);
        }

        updateCollision();
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

    Kerb::BodyShape CPhysics::getShape() const
    {
        return m_bodySettings.shape;
    }

    void CPhysics::appendDebugWireframe(Vertex::List& outVertices, const Vec4& inColor) const
    {
        if (!hasBody())
        {
            return;
        }

        const Transform  transform = Kerb::Engine::getInstance().getBodyTransform(m_body);
        const QuatFloat& rotation  = transform.getRotation().get();
        const Vec3       center    = transform.getTranslation();
        const Vec3       size      = m_bodySettings.bounds.getSize();

        if (m_bodySettings.shape == Kerb::BodyShape::Capsule)
        {
            const float radius     = std::max(0.05f, std::min(size.x, size.y) * 0.5f);
            const float halfHeight = std::max(0.0f, size.z * 0.5f - radius);

            Renderer::Debug::appendCapsule(outVertices, center, rotation, radius, halfHeight, inColor);

            return;
        }

        if (m_bodySettings.shape == Kerb::BodyShape::Polygon)
        {
            const Kerb::BodyPolygon polygon = Kerb::Engine::getInstance().getBodyPolygon(m_body);
            if (polygon.first.size() < 3 || polygon.second.empty())
            {
                return;
            }

            for (std::size_t i = 0; i + 2 < polygon.first.size(); i += 3)
            {
                const Vertex::Index i0 = polygon.first[i];
                const Vertex::Index i1 = polygon.first[i + 1];
                const Vertex::Index i2 = polygon.first[i + 2];
                if (i0 >= polygon.second.size() || i1 >= polygon.second.size() || i2 >= polygon.second.size())
                {
                    continue;
                }

                const Vec3& p0 = polygon.second[i0].position;
                const Vec3& p1 = polygon.second[i1].position;
                const Vec3& p2 = polygon.second[i2].position;

                Renderer::Debug::appendSegment(outVertices, p0, p1, inColor);
                Renderer::Debug::appendSegment(outVertices, p1, p2, inColor);
                Renderer::Debug::appendSegment(outVertices, p2, p0, inColor);
            }

            return;
        }

        Renderer::Debug::appendBox(
            outVertices,
            center,
            rotation,
            Vec3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f),
            inColor
        );
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
            Kerb::Engine& physics = Kerb::Engine::getInstance();
            physics.setBodyMotion(m_body, inType);
            syncCollisionSettings();
        }

        syncTickState();
    }

    void CPhysics::setMass(float inMass)
    {
        m_bodySettings.mass = inMass;

        if (hasBody())
        {
            Kerb::Engine::getInstance().setBodyMass(m_body, m_bodySettings.mass, m_bodySettings.massScale);
        }
    }

    void CPhysics::setMassScale(float inScale)
    {
        m_bodySettings.massScale = inScale;

        if (hasBody())
        {
            Kerb::Engine::getInstance().setBodyMass(m_body, m_bodySettings.mass, m_bodySettings.massScale);
        }
    }

    void CPhysics::setGravityFactor(float inFactor)
    {
        m_bodySettings.gravityFactor = inFactor;

        if (hasBody())
        {
            Kerb::Engine::getInstance().setBodyGravityFactor(m_body, m_bodySettings.gravityFactor);
        }
    }

    float CPhysics::getGravityFactor() const
    {
        if (hasBody())
        {
            return Kerb::Engine::getInstance().getBodyGravityFactor(m_body);
        }

        return m_bodySettings.gravityFactor;
    }

    void CPhysics::setObjectLayer(Kerb::ObjectLayer inLayer)
    {
        m_bodySettings.layer  = inLayer;
        m_bodySettings.preset = Kerb::CollisionPreset::Custom;

        if (hasBody())
        {
            syncCollisionSettings();
        }
    }

    void CPhysics::setCollisionPreset(Kerb::CollisionPreset inPreset)
    {
        const bool bWasSensor = m_bodySettings.bSensor;
        m_bodySettings.preset = inPreset;

        if (inPreset != Kerb::CollisionPreset::Custom)
        {
            const Kerb::CollisionPresetInfo info = Kerb::resolveCollisionPreset(inPreset, m_bodySettings.motion);
            m_bodySettings.layer                 = info.layer;
            m_bodySettings.bSensor               = info.bIsSensor;
        }

        if (!hasBody())
        {
            return;
        }

        if (bWasSensor != m_bodySettings.bSensor)
        {
            rebuildBody();

            return;
        }

        syncCollisionSettings();
    }

    void CPhysics::setCollisionGroup(const Kerb::CollisionGroup& inGroup)
    {
        m_bodySettings.group = inGroup;

        if (hasBody())
        {
            rebuildBody();
        }
    }

    void CPhysics::setSensor(bool bInSensor)
    {
        if (m_bodySettings.bSensor == bInSensor)
        {
            return;
        }

        m_bodySettings.bSensor = bInSensor;

        if (hasBody())
        {
            rebuildBody();
        }
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

    void CPhysics::updateCollision()
    {
        if (!canCollide())
        {
            return;
        }

        Actor* attachment = getParent<Actor>();
        if (!attachment || !attachment->canCollide())
        {
            return;
        }

        for (Actor* actor : getScene()->getActors())
        {
            if (actor == attachment || !actor->canCollide() || !actor->isCollidingWith(attachment))
            {
                continue;
            }

            attachment->collideWith(actor);
            actor->collideWith(attachment);
        }
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

    void CPhysics::rebuildBody()
    {
        const bool bWasActive = isActive() && hasBody();
        destroyBody();
        ensureBody();

        if (bWasActive && hasBody())
        {
            Kerb::Engine::getInstance().activateBody(m_body);
        }
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
            rebuildBody();
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

    void CPhysics::syncCollisionSettings()
    {
        if (!hasBody())
        {
            return;
        }

        Kerb::ObjectLayer layer = m_bodySettings.layer;
        if (m_bodySettings.preset != Kerb::CollisionPreset::Custom)
        {
            layer = Kerb::resolveCollisionPreset(m_bodySettings.preset, m_bodySettings.motion).layer;
        }
        else
        {
            layer = Kerb::resolveObjectLayer(m_bodySettings.layer, m_bodySettings.motion);
        }

        Kerb::Engine::getInstance().setBodyObjectLayer(m_body, layer);
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
