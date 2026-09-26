#include "Chicane/Runtime/Scene/Component/Physics.reflected.hpp"

#include <cmath>

#include "Chicane/Core/Math/Mat/Mat4.hpp"
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
            return Vec3::sZero();
        }

        return Vec3(max.x - min.x, max.y - min.y, max.z - min.z);
    }

    static bool changed(const Vec3& inLeft, const Vec3& inRight)
    {
        return std::abs(inLeft.x - inRight.x) > 0.01f || std::abs(inLeft.y - inRight.y) > 0.01f ||
               std::abs(inLeft.z - inRight.z) > 0.01f;
    }

    static Bounds3D scaledWorldBounds(const Bounds3D& inBounds, const Vec3& inCenter, const Vec3& inScale)
    {
        const Vec3 half = inBounds.getSize() * inScale * 0.5f;

        Vertex::List corners(2);
        corners[0].position = inCenter - half;
        corners[1].position = inCenter + half;

        Bounds3D bounds(corners);
        bounds.transform(Mat4(1.0f));

        return bounds;
    }

    CPhysics::CPhysics()
        : Component(),
          body({}),
          m_body(Kerb::Body::sInvalid()),
          m_syncedScale(Vec3::sOne()),
          m_syncedLocalSize(Vec3::sZero()),
          m_syncedRelativeTranslation(Vec3::sZero()),
          m_syncedRelativeRotation(Vec3::sZero()),
          m_actorToBody(Vec3::sZero()),
          m_bIsSyncingBody(false)
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

        if (body.motion == Kerb::MotionType::Kinematic)
        {
            applyBodyTransform();
            updateCollision();

            return;
        }

        if (body.motion != Kerb::MotionType::Dynamic)
        {
            return;
        }

        const Transform transform = Kerb::Engine::sInstance().getBodyTransform(m_body, true);
        const Vec3      location  = transform.getTranslation() - m_actorToBody;
        if (changed(location, m_parent->getTranslation()))
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

        Kerb::Engine::sInstance().activateBody(m_body);
    }

    void CPhysics::onDeactivation()
    {
        if (!hasBody())
        {
            return;
        }

        Kerb::Engine::sInstance().deactivateBody(m_body);
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

    void CPhysics::onPropertyEdited(const String& inName)
    {
        const std::vector<String> parts = inName.split('.');
        const String              name  = parts.empty() ? inName : parts.back();

        if (name.equals("shape"))
        {
            setShape(body.shape);

            return;
        }

        if (name.equals("motion"))
        {
            setMotion(body.motion);

            return;
        }

        if (name.equals("mass"))
        {
            setMass(body.mass);

            return;
        }

        if (name.equals("massScale"))
        {
            setMassScale(body.massScale);

            return;
        }

        if (name.equals("gravityFactor"))
        {
            setGravityFactor(body.gravityFactor);

            return;
        }

        if (name.equals("layer"))
        {
            setObjectLayer(body.layer);

            return;
        }

        if (name.equals("preset"))
        {
            setCollisionPreset(body.preset);

            return;
        }

        if (name.equals("bSensor"))
        {
            setSensor(body.bIsSensor);
        }
    }

    Kerb::BodyShape CPhysics::getShape() const
    {
        return body.shape;
    }

    void CPhysics::appendDebugWireframe(Vertex::List& outVertices, const Vec4& inColor) const
    {
        if (!hasBody())
        {
            return;
        }

        const Transform  transform = Kerb::Engine::sInstance().getBodyTransform(m_body);
        const QuatFloat& rotation  = transform.getRotation().get();
        const Vec3       center    = transform.getTranslation();
        const Vec3       size      = body.bounds.getSize();

        if (body.shape == Kerb::BodyShape::Capsule)
        {
            const float radius     = std::max(0.05f, std::min(size.x, size.y) * 0.5f);
            const float halfHeight = std::max(0.0f, size.z * 0.5f - radius);

            Renderer::Debug::appendCapsule(outVertices, center, rotation, radius, halfHeight, inColor);

            return;
        }

        if (body.shape == Kerb::BodyShape::Polygon)
        {
            const Kerb::BodyPolygon polygon = Kerb::Engine::sInstance().getBodyPolygon(m_body);
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
        body.shape = inType;

        if (hasBody())
        {
            rebuildBody();
        }
    }

    void CPhysics::setShape(const Kerb::BodyPolygon& inPolygon)
    {
        body.shape   = Kerb::BodyShape::Polygon;
        body.polygon = inPolygon;

        if (hasBody())
        {
            rebuildBody();
        }
    }

    void CPhysics::setMotion(Kerb::MotionType inType)
    {
        body.motion = inType;

        if (hasBody())
        {
            Kerb::Engine& physics = Kerb::Engine::sInstance();
            physics.setBodyMotion(m_body, inType);
            syncCollisionSettings();
        }

        syncTickState();
    }

    void CPhysics::setMass(float inMass)
    {
        body.mass = inMass;

        if (hasBody())
        {
            Kerb::Engine::sInstance().setBodyMass(m_body, body.mass, body.massScale);
        }
    }

    void CPhysics::setMassScale(float inScale)
    {
        body.massScale = inScale;

        if (hasBody())
        {
            Kerb::Engine::sInstance().setBodyMass(m_body, body.mass, body.massScale);
        }
    }

    void CPhysics::setGravityFactor(float inFactor)
    {
        body.gravityFactor = inFactor;

        if (hasBody())
        {
            Kerb::Engine::sInstance().setBodyGravityFactor(m_body, body.gravityFactor);
        }
    }

    float CPhysics::getGravityFactor() const
    {
        if (hasBody())
        {
            return Kerb::Engine::sInstance().getBodyGravityFactor(m_body);
        }

        return body.gravityFactor;
    }

    void CPhysics::setObjectLayer(Kerb::ObjectLayer inLayer)
    {
        body.layer  = inLayer;
        body.preset = Kerb::CollisionPreset::Custom;

        if (hasBody())
        {
            syncCollisionSettings();
        }
    }

    void CPhysics::setCollisionPreset(Kerb::CollisionPreset inPreset)
    {
        const bool bWasSensor = body.bIsSensor;
        body.preset           = inPreset;

        if (inPreset != Kerb::CollisionPreset::Custom)
        {
            const Kerb::CollisionPresetInfo info = Kerb::resolveCollisionPreset(inPreset, body.motion);
            body.layer                           = info.layer;
            body.bIsSensor                       = info.bIsSensor;
        }

        if (!hasBody())
        {
            return;
        }

        if (bWasSensor != body.bIsSensor)
        {
            rebuildBody();

            return;
        }

        syncCollisionSettings();
    }

    void CPhysics::setCollisionGroup(const Kerb::CollisionGroup& inGroup)
    {
        body.group = inGroup;

        if (hasBody())
        {
            rebuildBody();
        }
    }

    void CPhysics::setSensor(bool bInSensor)
    {
        if (body.bIsSensor == bInSensor)
        {
            return;
        }

        body.bIsSensor = bInSensor;

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

        Kerb::Engine::sInstance().setBodyTransform(m_body, makeBodyTransform(inLocation));
    }

    void CPhysics::moveBy(const Vec3& inOffset)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        const Vec3 current = Kerb::Engine::sInstance().getBodyTransform(m_body).getTranslation();
        moveTo(current + inOffset);
    }

    Vec3 CPhysics::getLinearVelocity() const
    {
        if (!hasBody())
        {
            return Vec3::sZero();
        }

        return Kerb::Engine::sInstance().getBodyLinearVelocity(m_body);
    }

    void CPhysics::setLinearVelocity(const Vec3& inVelocity)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        Kerb::Engine::sInstance().setBodyLinearVelocity(m_body, inVelocity);
    }

    void CPhysics::setHorizontalVelocity(const Vec3& inVelocity)
    {
        if (!hasBody() || !isAttached())
        {
            return;
        }

        Kerb::Engine::sInstance().setBodyHorizontalVelocity(m_body, inVelocity);
    }

    void CPhysics::addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation)
    {
        if (!canCollide())
        {
            return;
        }

        Kerb::Engine::sInstance().addBodyImpulse(m_body, inDirection, inForce, inLocation);
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

        captureSyncedTransform();

        body.bounds   = scaledWorldBounds(m_parent->getBounds(), colliderCenter(), getRelativeScale());
        m_actorToBody = colliderCenter() - m_parent->getTranslation();
        m_body        = Kerb::Engine::sInstance().createBody(body);

        if (!hasBody())
        {
            syncTickState();

            return;
        }

        if (isActive())
        {
            Kerb::Engine::sInstance().activateBody(m_body);
        }

        applyBodyTransform();
        syncTickState();
    }

    void CPhysics::destroyBody()
    {
        if (!hasBody())
        {
            return;
        }

        Kerb::Engine::sInstance().destroyBody(m_body);
        m_body = Kerb::Body::sInvalid();

        syncTickState();
    }

    void CPhysics::rebuildBody()
    {
        const bool bWasActive = isActive() && hasBody();
        destroyBody();
        ensureBody();

        if (bWasActive && hasBody())
        {
            Kerb::Engine::sInstance().activateBody(m_body);
        }
    }

    void CPhysics::syncBody()
    {
        if (m_bIsSyncingBody || !isAttached() || !hasBody() || !m_parent)
        {
            return;
        }

        const Vec3 scale     = getScale();
        const Vec3 localSize = localBoundsSize(m_parent->getBounds());
        if (changed(scale, m_syncedScale) || changed(localSize, m_syncedLocalSize))
        {
            m_bIsSyncingBody = true;
            rebuildBody();
            m_bIsSyncingBody = false;

            return;
        }

        const bool bRelativeMoved = changed(getRelativeTranslation(), m_syncedRelativeTranslation) ||
                                    changed(getRelativeRotation().getAngles(), m_syncedRelativeRotation);

        if (body.motion != Kerb::MotionType::Dynamic || bRelativeMoved)
        {
            applyBodyTransform();
            captureSyncedTransform();
        }
    }

    void CPhysics::syncTickState()
    {
        setCanTick(hasBody() && body.motion != Kerb::MotionType::Static);
    }

    void CPhysics::syncCollisionSettings()
    {
        if (!hasBody())
        {
            return;
        }

        Kerb::ObjectLayer resolved = body.layer;
        if (body.preset != Kerb::CollisionPreset::Custom)
        {
            resolved = Kerb::resolveCollisionPreset(body.preset, body.motion).layer;
        }
        else
        {
            resolved = Kerb::resolveObjectLayer(body.layer, body.motion);
        }

        Kerb::Engine::sInstance().setBodyObjectLayer(m_body, resolved);
    }

    void CPhysics::captureSyncedTransform()
    {
        m_syncedScale               = getScale();
        m_syncedLocalSize           = m_parent ? localBoundsSize(m_parent->getBounds()) : Vec3::sZero();
        m_syncedRelativeTranslation = getRelativeTranslation();
        m_syncedRelativeRotation    = getRelativeRotation().getAngles();
    }

    Vec3 CPhysics::colliderCenter() const
    {
        if (!m_parent)
        {
            return getTranslation();
        }

        return m_parent->getBounds().getCenter() + (getTranslation() - getAbsoluteTranslation());
    }

    void CPhysics::applyBodyTransform()
    {
        if (!hasBody() || !m_parent)
        {
            return;
        }

        m_actorToBody = colliderCenter() - m_parent->getTranslation();
        Kerb::Engine::sInstance().setBodyTransform(m_body, makeBodyTransform(colliderCenter()));
    }

    Transform CPhysics::makeBodyTransform(const Vec3& inLocation) const
    {
        Transform transform;
        transform.setScale(getScale());
        transform.setRotation(getRotation());
        transform.setTranslation(inLocation);

        return transform;
    }
}
