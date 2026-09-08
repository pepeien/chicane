#include "Chicane/Kerb/Engine.hpp"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/CollisionGroup.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Kerb/Collision/Preset/Info.hpp"
#include "Chicane/Kerb/Gravity.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

#include "Convert.hpp"

namespace Chicane
{
    namespace Kerb
    {
        namespace BroadPhaseLayer
        {
            static constexpr JPH::BroadPhaseLayer NonMoving(0);
            static constexpr JPH::BroadPhaseLayer Moving(1);

            static constexpr std::uint32_t Count = 2;
        }

        static constexpr std::uint32_t OBJECT_LAYER_COUNT       = static_cast<std::uint32_t>(ObjectLayer::Count);
        static constexpr std::uint32_t MAX_COLLISION_SUB_GROUPS = 32;
        static constexpr std::uint32_t MAX_BODIES               = 65536;
        static constexpr std::uint32_t MAX_BODY_MUTEXES         = 0;
        static constexpr std::uint32_t MAX_BODY_PAIRS           = MAX_BODIES;
        static constexpr std::uint32_t MAX_CONTACT_CONSTRAINTS  = 10240;
        static constexpr float         FIXED_STEP               = 1.0f / 60.0f;
        static constexpr int           COLLISION_STEPS          = 1;
        static constexpr int           MAX_STEPS_PER_TICK       = 4;

        JPH::BodyID toId(Body inBody)
        {
            return JPH::BodyID(inBody.value());
        }

        Body toBody(JPH::BodyID inId)
        {
            return Body(inId.GetIndexAndSequenceNumber());
        }

        JPH::ObjectLayer toPhysicsObjectLayer(ObjectLayer inLayer)
        {
            return static_cast<JPH::ObjectLayer>(inLayer);
        }

        bool isNonMovingLayer(ObjectLayer inLayer)
        {
            return inLayer == ObjectLayer::NonMoving || inLayer == ObjectLayer::NoCollision;
        }

        ObjectLayer resolveCreateLayer(const BodyCreateInfo& inCreateInfo)
        {
            if (inCreateInfo.preset != CollisionPreset::Custom)
            {
                return resolveCollisionPreset(inCreateInfo.preset, inCreateInfo.motion).layer;
            }

            return resolveObjectLayer(inCreateInfo.layer, inCreateInfo.motion);
        }

        bool resolveCreateSensor(const BodyCreateInfo& inCreateInfo)
        {
            if (inCreateInfo.preset != CollisionPreset::Custom)
            {
                return resolveCollisionPreset(inCreateInfo.preset, inCreateInfo.motion).bIsSensor ||
                       inCreateInfo.bSensor;
            }

            return inCreateInfo.bSensor || inCreateInfo.layer == ObjectLayer::Trigger;
        }

        float resolveCreateMass(const BodyCreateInfo& inCreateInfo)
        {
            return std::max(0.1f, inCreateInfo.mass * std::max(0.0f, inCreateInfo.massScale));
        }

        void enableLayerPair(JPH::ObjectLayerPairFilterTable& inTable, ObjectLayer inA, ObjectLayer inB)
        {
            const JPH::ObjectLayer a = toPhysicsObjectLayer(inA);
            const JPH::ObjectLayer b = toPhysicsObjectLayer(inB);
            inTable.EnableCollision(a, b);
            if (a != b)
            {
                inTable.EnableCollision(b, a);
            }
        }

        void setupObjectLayerMatrix(JPH::ObjectLayerPairFilterTable& inTable)
        {
            // WorldStatic / NonMoving collides with everything dynamic-ish, not other static.
            enableLayerPair(inTable, ObjectLayer::NonMoving, ObjectLayer::Moving);
            enableLayerPair(inTable, ObjectLayer::NonMoving, ObjectLayer::Pawn);
            enableLayerPair(inTable, ObjectLayer::NonMoving, ObjectLayer::PhysicsBody);
            enableLayerPair(inTable, ObjectLayer::NonMoving, ObjectLayer::Projectile);
            enableLayerPair(inTable, ObjectLayer::NonMoving, ObjectLayer::Trigger);
            enableLayerPair(inTable, ObjectLayer::NonMoving, ObjectLayer::Debris);

            // WorldDynamic / Moving
            enableLayerPair(inTable, ObjectLayer::Moving, ObjectLayer::Moving);
            enableLayerPair(inTable, ObjectLayer::Moving, ObjectLayer::Pawn);
            enableLayerPair(inTable, ObjectLayer::Moving, ObjectLayer::PhysicsBody);
            enableLayerPair(inTable, ObjectLayer::Moving, ObjectLayer::Projectile);
            enableLayerPair(inTable, ObjectLayer::Moving, ObjectLayer::Trigger);
            enableLayerPair(inTable, ObjectLayer::Moving, ObjectLayer::Debris);

            // Pawn
            enableLayerPair(inTable, ObjectLayer::Pawn, ObjectLayer::Pawn);
            enableLayerPair(inTable, ObjectLayer::Pawn, ObjectLayer::PhysicsBody);
            enableLayerPair(inTable, ObjectLayer::Pawn, ObjectLayer::Projectile);
            enableLayerPair(inTable, ObjectLayer::Pawn, ObjectLayer::Trigger);
            // Pawn ignores Debris (debris should not shove characters)

            // PhysicsBody
            enableLayerPair(inTable, ObjectLayer::PhysicsBody, ObjectLayer::PhysicsBody);
            enableLayerPair(inTable, ObjectLayer::PhysicsBody, ObjectLayer::Projectile);
            enableLayerPair(inTable, ObjectLayer::PhysicsBody, ObjectLayer::Trigger);
            enableLayerPair(inTable, ObjectLayer::PhysicsBody, ObjectLayer::Debris);

            // Projectile ignores other projectiles
            enableLayerPair(inTable, ObjectLayer::Projectile, ObjectLayer::Trigger);
            enableLayerPair(inTable, ObjectLayer::Projectile, ObjectLayer::Debris);

            // Trigger overlaps/contacts most things; ignore other triggers
            enableLayerPair(inTable, ObjectLayer::Trigger, ObjectLayer::Debris);
        }

        Hash::Value hashPolygon(const BodyPolygon& inPolygon)
        {
            Vertex::Positions positions;
            positions.reserve(inPolygon.second.size());

            for (const Vertex& vertex : inPolygon.second)
            {
                positions.push_back(vertex.position);
            }

            Hash::Subject data;
            data.data = inPolygon.first.data();
            data.size = inPolygon.first.size() * sizeof(Vertex::Index);

            Hash::Subject seed;
            seed.data = positions.data();
            seed.size = positions.size() * sizeof(Vertex::Position);

            return Hash::generate(data, seed);
        }

        struct Engine::Implementation
        {
            Implementation()
                : tempAllocator(10 * 1024 * 1024),
                  broadLayer(OBJECT_LAYER_COUNT, BroadPhaseLayer::Count),
                  objectLayer(OBJECT_LAYER_COUNT),
                  groupFilter(new JPH::GroupFilterTable(MAX_COLLISION_SUB_GROUPS)),
                  gravity(getEarthGravity()),
                  accumulator(0.0f)
            {
                threadPool.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, -1);

                for (std::uint32_t i = 0; i < OBJECT_LAYER_COUNT; ++i)
                {
                    const ObjectLayer layer = static_cast<ObjectLayer>(i);
                    broadLayer.MapObjectToBroadPhaseLayer(
                        toPhysicsObjectLayer(layer),
                        isNonMovingLayer(layer) ? BroadPhaseLayer::NonMoving : BroadPhaseLayer::Moving
                    );
                }

                setupObjectLayerMatrix(objectLayer);

                objectVsBroadPhaseLayer = std::make_unique<JPH::ObjectVsBroadPhaseLayerFilterTable>(
                    broadLayer,
                    BroadPhaseLayer::Count,
                    objectLayer,
                    OBJECT_LAYER_COUNT
                );

                system.Init(
                    MAX_BODIES,
                    MAX_BODY_MUTEXES,
                    MAX_BODY_PAIRS,
                    MAX_CONTACT_CONSTRAINTS,
                    broadLayer,
                    *objectVsBroadPhaseLayer,
                    objectLayer
                );
                system.SetGravity(Convert::toPhysicsPosition(gravity));
            }

            void applyGravity(const Vec3& inValue)
            {
                gravity = inValue;

                system.SetGravity(Convert::toPhysicsPosition(gravity));
            }

            JPH::BodyInterface& bodies() { return system.GetBodyInterface(); }

            const JPH::BodyInterface& bodies() const { return system.GetBodyInterface(); }

            JPH::RefConst<JPH::Shape> createShape(const BodyCreateInfo& inCreateInfo);
            void applyPendingWrites();

            struct PendingWrite
            {
                enum class Kind
                {
                    Velocity,
                    HorizontalVelocity,
                    Impulse
                };

                Kind        kind     = Kind::Velocity;
                JPH::BodyID id       = JPH::BodyID();
                JPH::Vec3   vector   = JPH::Vec3::sZero();
                JPH::Vec3   location = JPH::Vec3::sZero();
            };

            JPH::JobSystemThreadPool                                   threadPool;
            JPH::TempAllocatorImpl                                     tempAllocator;
            JPH::BroadPhaseLayerInterfaceTable                         broadLayer;
            JPH::ObjectLayerPairFilterTable                            objectLayer;
            std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilterTable>   objectVsBroadPhaseLayer;
            JPH::RefConst<JPH::GroupFilterTable>                       groupFilter;
            JPH::PhysicsSystem                                         system;
            std::unordered_map<Hash::Value, JPH::RefConst<JPH::Shape>> shapes;
            std::vector<JPH::BodyID>                                   ids;
            std::mutex                                                 pendingMutex;
            std::vector<PendingWrite>                                  pending;
            std::unordered_map<std::uint32_t, JPH::Vec3>               horizontalWish;
            std::unordered_map<std::uint32_t, JPH::RVec3>              previousPosition;
            Vec3                                                       gravity;
            bool                                                       bBroadPhaseDirty = false;
            float                                                      accumulator;
        };

        JPH::RefConst<JPH::Shape> makeBox(const Vec3& inSize)
        {
            const JPH::Vec3 half = Convert::toPhysicsSize(inSize * 0.5f);
            const JPH::Vec3 extents(
                std::max(0.05f, half.GetX()),
                std::max(0.05f, half.GetY()),
                std::max(0.05f, half.GetZ())
            );
            const float convexRadius = std::min(0.05f, extents.ReduceMin() * 0.25f);

            const JPH::BoxShapeSettings   settings(extents, convexRadius);
            const JPH::Shape::ShapeResult result = settings.Create();
            if (result.HasError())
            {
                return new JPH::BoxShape(JPH::Vec3::sReplicate(0.5f), convexRadius);
            }

            return result.Get();
        }

        JPH::RefConst<JPH::Shape> makeCapsule(const Vec3& inSize)
        {
            const float radius     = std::max(0.05f, std::min(inSize.x, inSize.y) * 0.5f);
            const float halfHeight = std::max(0.0f, inSize.z * 0.5f - radius);

            const JPH::CapsuleShapeSettings settings(halfHeight, radius);
            const JPH::Shape::ShapeResult   result = settings.Create();
            if (result.HasError())
            {
                return makeBox(inSize);
            }

            return result.Get();
        }

        JPH::RefConst<JPH::Shape> makeHull(const BodyPolygon& inPolygon)
        {
            if (inPolygon.second.size() < 4)
            {
                return {};
            }

            JPH::Array<JPH::Vec3> points;
            points.reserve(static_cast<JPH::uint>(inPolygon.second.size()));

            for (const Vertex& vertex : inPolygon.second)
            {
                points.push_back(Convert::toPhysicsPosition(vertex.position));
            }

            const JPH::ConvexHullShapeSettings settings(points);
            const JPH::Shape::ShapeResult      result = settings.Create();
            if (result.HasError())
            {
                return {};
            }

            return result.Get();
        }

        JPH::RefConst<JPH::Shape> Engine::Implementation::createShape(const BodyCreateInfo& inCreateInfo)
        {
            if (inCreateInfo.shape == BodyShape::Polygon)
            {
                if (inCreateInfo.polygon.second.size() < 4)
                {
                    return makeBox(inCreateInfo.bounds.getSize());
                }
                const Hash::Value hash  = hashPolygon(inCreateInfo.polygon);
                const auto        found = shapes.find(hash);
                if (found != shapes.end())
                {
                    return found->second;
                }

                JPH::RefConst<JPH::Shape> hull = makeHull(inCreateInfo.polygon);
                if (hull == nullptr)
                {
                    hull = makeBox(inCreateInfo.bounds.getSize());
                }

                shapes.insert_or_assign(hash, hull);

                return hull;
            }

            if (inCreateInfo.shape == BodyShape::Capsule)
            {
                return makeCapsule(inCreateInfo.bounds.getSize());
            }

            return makeBox(inCreateInfo.bounds.getSize());
        }

        Engine& Engine::getInstance()
        {
            static Engine instance;

            return instance;
        }

        const Vec3& Engine::getZeroGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Zero);

            return value;
        }

        const Vec3& Engine::getMercuryGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Mercury);

            return value;
        }

        const Vec3& Engine::getVenusGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Venus);

            return value;
        }

        const Vec3& Engine::getEarthGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Earth);

            return value;
        }

        const Vec3& Engine::getMoonGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Moon);

            return value;
        }

        const Vec3& Engine::getMarsGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Mars);

            return value;
        }

        const Vec3& Engine::getJupiterGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Jupiter);

            return value;
        }

        const Vec3& Engine::getSaturnGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Saturn);

            return value;
        }

        const Vec3& Engine::getUranusGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Uranus);

            return value;
        }

        const Vec3& Engine::getNeptuneGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Neptune);

            return value;
        }

        const Vec3& Engine::getPlutoGravity()
        {
            static const Vec3 value = Gravity::down(Gravity::Pluto);

            return value;
        }

        const Vec3& Engine::getPlanetGravity(Planet inPlanet)
        {
            switch (inPlanet)
            {
            case Planet::Mercury:
                return getMercuryGravity();

            case Planet::Venus:
                return getVenusGravity();

            case Planet::Moon:
                return getMoonGravity();

            case Planet::Mars:
                return getMarsGravity();

            case Planet::Jupiter:
                return getJupiterGravity();

            case Planet::Saturn:
                return getSaturnGravity();

            case Planet::Uranus:
                return getUranusGravity();

            case Planet::Neptune:
                return getNeptuneGravity();

            case Planet::Pluto:
                return getPlutoGravity();

            case Planet::Earth:
            default:
                return getEarthGravity();
            }
        }

        void Engine::setZeroGravity()
        {
            setGravity(getZeroGravity());
        }

        void Engine::setGravity(Planet inPlanet)
        {
            setGravity(getPlanetGravity(inPlanet));
        }

        void Engine::setGravity(const Vec3& inValue)
        {
            Engine& engine = getInstance();
            if (!engine.m_implementation)
            {
                return;
            }

            engine.m_implementation->applyGravity(inValue);
        }

        Engine::Engine()
            : m_implementation(std::make_unique<Implementation>())
        {}

        Engine::~Engine()
        {
            if (!m_implementation)
            {
                return;
            }

            JPH::BodyInterface& interface = m_implementation->bodies();
            for (const JPH::BodyID id : m_implementation->ids)
            {
                if (id.IsInvalid())
                {
                    continue;
                }

                if (interface.IsAdded(id))
                {
                    interface.RemoveBody(id);
                }

                interface.DestroyBody(id);
            }

            m_implementation->ids.clear();
            m_implementation.reset();
        }

        const Vec3& Engine::getGravity() const
        {
            if (!m_implementation)
            {
                return getEarthGravity();
            }

            return m_implementation->gravity;
        }

        void Engine::Implementation::applyPendingWrites()
        {
            std::vector<PendingWrite> writes;
            {
                std::lock_guard<std::mutex> lock(pendingMutex);
                writes.swap(pending);
            }

            JPH::BodyInterface& interface = bodies();
            for (const PendingWrite& write : writes)
            {
                if (write.id.IsInvalid() || !interface.IsAdded(write.id))
                {
                    continue;
                }

                if (write.kind == PendingWrite::Kind::Impulse)
                {
                    interface.AddImpulse(write.id, write.vector, write.location);

                    continue;
                }

                if (write.kind == PendingWrite::Kind::HorizontalVelocity)
                {
                    horizontalWish[write.id.GetIndexAndSequenceNumber()] = write.vector;

                    continue;
                }

                horizontalWish.erase(write.id.GetIndexAndSequenceNumber());
                interface.SetLinearVelocity(write.id, write.vector);
            }
        }

        void Engine::tick(float inDeltaTime)
        {
            if (!m_implementation)
            {
                return;
            }

            m_implementation->applyPendingWrites();
            m_implementation->accumulator += std::min(std::max(inDeltaTime, 0.0f), 0.25f);

            if (m_implementation->bBroadPhaseDirty)
            {
                m_implementation->system.OptimizeBroadPhase();
                m_implementation->bBroadPhaseDirty = false;
            }

            int steps = 0;
            while (m_implementation->accumulator >= FIXED_STEP && steps < MAX_STEPS_PER_TICK)
            {
                JPH::BodyInterface& interface = m_implementation->bodies();
                for (const JPH::BodyID id : m_implementation->ids)
                {
                    if (id.IsInvalid() || !interface.IsAdded(id))
                    {
                        continue;
                    }

                    m_implementation->previousPosition[id.GetIndexAndSequenceNumber()] = interface.GetPosition(id);
                }

                for (const auto& [key, wish] : m_implementation->horizontalWish)
                {
                    const JPH::BodyID id(key);
                    if (id.IsInvalid() || !interface.IsAdded(id))
                    {
                        continue;
                    }

                    JPH::Vec3 velocity = interface.GetLinearVelocity(id);
                    velocity.SetX(wish.GetX());
                    velocity.SetZ(wish.GetZ());
                    interface.SetLinearVelocity(id, velocity);
                }

                m_implementation->system.Update(
                    FIXED_STEP,
                    COLLISION_STEPS,
                    &m_implementation->tempAllocator,
                    &m_implementation->threadPool
                );
                m_implementation->accumulator -= FIXED_STEP;
                ++steps;
            }

            m_implementation->horizontalWish.clear();

            if (steps >= MAX_STEPS_PER_TICK)
            {
                m_implementation->accumulator = 0.0f;
            }
        }

        Body Engine::createBody(const BodyCreateInfo& inCreateInfo)
        {
            if (!m_implementation)
            {
                return Body::invalid();
            }

            const JPH::RefConst<JPH::Shape> shape = m_implementation->createShape(inCreateInfo);
            if (shape == nullptr)
            {
                return Body::invalid();
            }

            const bool        bIsStatic = inCreateInfo.motion == MotionType::Static;
            const ObjectLayer layer     = resolveCreateLayer(inCreateInfo);
            const bool        bSensor   = resolveCreateSensor(inCreateInfo);

            JPH::BodyCreationSettings settings(
                shape,
                Convert::toPhysicsPosition(inCreateInfo.bounds.getCenter()),
                JPH::Quat::sIdentity(),
                Convert::toPhysicsMotion(inCreateInfo.motion),
                toPhysicsObjectLayer(layer)
            );
            settings.mAllowSleeping                = bIsStatic;
            settings.mAllowDynamicOrKinematic      = !bIsStatic;
            settings.mGravityFactor                = inCreateInfo.gravityFactor;
            settings.mIsSensor                     = bSensor;
            settings.mOverrideMassProperties       = JPH::EOverrideMassProperties::CalculateInertia;
            settings.mMassPropertiesOverride.mMass = resolveCreateMass(inCreateInfo);

            if (inCreateInfo.group.isValid() && m_implementation->groupFilter != nullptr)
            {
                settings.mCollisionGroup = JPH::CollisionGroup(
                    m_implementation->groupFilter,
                    inCreateInfo.group.group,
                    inCreateInfo.group.subGroup
                );
            }

            if (inCreateInfo.shape == BodyShape::Capsule && !bIsStatic)
            {
                settings.mFriction       = 0.0f;
                settings.mRestitution    = 0.0f;
                settings.mLinearDamping  = 0.0f;
                settings.mAngularDamping = 0.0f;
                settings.mMotionQuality  = JPH::EMotionQuality::LinearCast;
                settings.mAllowedDOFs =
                    JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ;
            }
            else
            {
                settings.mMotionQuality = bIsStatic ? JPH::EMotionQuality::Discrete : JPH::EMotionQuality::LinearCast;
            }

            JPH::Body* created = m_implementation->bodies().CreateBody(settings);
            if (!created)
            {
                return Body::invalid();
            }

            const JPH::BodyID id = created->GetID();
            m_implementation->ids.push_back(id);

            return toBody(id);
        }

        void Engine::activateBody(Body inBody)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            JPH::BodyInterface& interface = m_implementation->bodies();
            if (!interface.IsAdded(id))
            {
                interface.AddBody(id, JPH::EActivation::Activate);
                m_implementation->bBroadPhaseDirty = true;

                return;
            }

            interface.ActivateBody(id);
        }

        void Engine::deactivateBody(Body inBody)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            JPH::BodyInterface& interface = m_implementation->bodies();
            if (interface.IsAdded(id))
            {
                interface.RemoveBody(id);
            }
        }

        void Engine::destroyBody(Body inBody)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            JPH::BodyInterface& interface = m_implementation->bodies();
            if (interface.IsAdded(id))
            {
                interface.RemoveBody(id);
            }

            interface.DestroyBody(id);
            m_implementation->horizontalWish.erase(id.GetIndexAndSequenceNumber());
            m_implementation->previousPosition.erase(id.GetIndexAndSequenceNumber());

            auto& ids = m_implementation->ids;
            ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
        }

        void Engine::setBodyMotion(Body inBody, MotionType inType)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            m_implementation->bodies().SetMotionType(id, Convert::toPhysicsMotion(inType), JPH::EActivation::Activate);
        }

        void Engine::setBodyObjectLayer(Body inBody, ObjectLayer inLayer)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation || inLayer == ObjectLayer::Auto ||
                static_cast<std::uint8_t>(inLayer) >= static_cast<std::uint8_t>(ObjectLayer::Count))
            {
                return;
            }

            m_implementation->bodies().SetObjectLayer(id, toPhysicsObjectLayer(inLayer));
        }

        void Engine::setBodyMass(Body inBody, float inMass, float inMassScale)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            JPH::BodyInterface& interface = m_implementation->bodies();
            if (!interface.IsAdded(id))
            {
                return;
            }

            const JPH::BodyLockWrite lock(m_implementation->system.GetBodyLockInterface(), id);
            if (!lock.Succeeded())
            {
                return;
            }

            JPH::Body& body = lock.GetBody();
            if (!body.IsDynamic())
            {
                return;
            }

            const float mass = std::max(0.1f, inMass * std::max(0.0f, inMassScale));
            body.GetMotionProperties()->ScaleToMass(mass);
        }

        void Engine::setBodyGravityFactor(Body inBody, float inFactor)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            m_implementation->bodies().SetGravityFactor(id, inFactor);
        }

        float Engine::getBodyGravityFactor(Body inBody) const
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return Gravity::FactorFull;
            }

            return m_implementation->bodies().GetGravityFactor(id);
        }

        void Engine::addBodyImpulse(Body inBody, const Vec3& inDirection, float inForce, const Vec3& inLocation)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(m_implementation->pendingMutex);
            m_implementation->pending.push_back(
                {Implementation::PendingWrite::Kind::Impulse,
                 id,
                 Convert::toPhysicsPosition(inDirection * inForce),
                 Convert::toPhysicsPosition(inLocation)}
            );
        }

        Vec3 Engine::getBodyLinearVelocity(Body inBody) const
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation || !m_implementation->bodies().IsAdded(id))
            {
                return Vec3::Zero();
            }

            return Convert::toEnginePosition(m_implementation->bodies().GetLinearVelocity(id));
        }

        void Engine::setBodyLinearVelocity(Body inBody, const Vec3& inVelocity)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(m_implementation->pendingMutex);
            m_implementation->pending.push_back(
                {Implementation::PendingWrite::Kind::Velocity,
                 id,
                 Convert::toPhysicsPosition(inVelocity),
                 JPH::Vec3::sZero()}
            );
        }

        void Engine::setBodyHorizontalVelocity(Body inBody, const Vec3& inVelocity)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            std::lock_guard<std::mutex> lock(m_implementation->pendingMutex);
            m_implementation->pending.push_back(
                {Implementation::PendingWrite::Kind::HorizontalVelocity,
                 id,
                 Convert::toPhysicsPosition(Vec3(inVelocity.x, inVelocity.y, 0.0f)),
                 JPH::Vec3::sZero()}
            );
        }

        Transform Engine::getBodyTransform(Body inBody, bool bInInterpolate) const
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return {};
            }

            const JPH::BodyInterface& interface = m_implementation->bodies();
            const JPH::RMat44         transform = interface.GetWorldTransform(id);
            JPH::RVec3                position  = transform.GetTranslation();

            if (bInInterpolate)
            {
                const auto found = m_implementation->previousPosition.find(id.GetIndexAndSequenceNumber());
                if (found != m_implementation->previousPosition.end())
                {
                    const float alpha = std::clamp(m_implementation->accumulator / FIXED_STEP, 0.0f, 1.0f);
                    position          = found->second * (1.0f - alpha) + position * alpha;
                }
            }

            Transform result;
            result.setTranslation(Convert::toEnginePosition(JPH::Vec3(position)));
            result.setRotation(Rotator(Convert::toEngineRotation(transform.GetQuaternion())));

            return result;
        }

        void Engine::setBodyTransform(Body inBody, const Transform& inValue)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            m_implementation->bodies().SetPositionAndRotation(
                id,
                Convert::toPhysicsPosition(inValue.getTranslation()),
                Convert::toPhysicsRotation(inValue.getRotation().get()),
                JPH::EActivation::Activate
            );
        }

        BodyPolygon Engine::getBodyPolygon(Body inBody) const
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return {};
            }

            const JPH::TransformedShape shape = m_implementation->bodies().GetTransformedShape(id);

            JPH::Shape::GetTrianglesContext context;
            shape.GetTrianglesStart(context, shape.GetWorldSpaceBounds(), JPH::RVec3::sZero());

            constexpr int stepSize = 256;
            JPH::Float3   vertices[stepSize * 3];

            BodyPolygon result;

            while (true)
            {
                const int triangleCount = shape.GetTrianglesNext(context, stepSize, vertices);
                if (triangleCount <= 0)
                {
                    break;
                }

                for (int i = 0; i < triangleCount; ++i)
                {
                    const std::uint32_t offset = static_cast<std::uint32_t>(result.second.size());

                    for (int j = 0; j < 3; ++j)
                    {
                        result.first.push_back(offset + static_cast<std::uint32_t>(j));

                        const JPH::Float3& point = vertices[i * 3 + j];

                        Vertex vertex;
                        vertex.position = Convert::toEnginePosition(JPH::Vec3(point.x, point.y, point.z));
                        vertex.color    = Vec4(255.0f);

                        result.second.push_back(vertex);
                    }
                }
            }

            return result;
        }
    }
}
