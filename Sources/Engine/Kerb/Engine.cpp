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
#include <Jolt/Physics/Collision/ContactListener.h>
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
#include "Chicane/Core/Tracer.hpp"
#include "Chicane/Core/Mailbox.hpp"
#include "Chicane/Kerb/Collision/Preset/Info.hpp"
#include "Chicane/Kerb/Gravity.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

#include "Convert.hpp"

namespace Chicane
{
    namespace Kerb
    {
        enum class PendingWriteKind : std::uint8_t
        {
            Velocity,
            HorizontalVelocity,
            Impulse
        };

        struct PendingWrite
        {
        public:
            PendingWriteKind kind     = PendingWriteKind::Velocity;
            JPH::BodyID      id       = JPH::BodyID();
            JPH::Vec3        vector   = JPH::Vec3::sZero();
            JPH::Vec3        location = JPH::Vec3::sZero();
        };

        struct ContactRecord
        {
        public:
            std::uint32_t first  = 0;
            std::uint32_t second = 0;
        };

        class ContactRelay final : public JPH::ContactListener
        {
        public:
            void OnContactAdded(
                const JPH::Body& inBody1,
                const JPH::Body& inBody2,
                const JPH::ContactManifold&,
                JPH::ContactSettings&
            ) override
            {
                push(inBody1, inBody2);
            }

            void OnContactPersisted(
                const JPH::Body& inBody1,
                const JPH::Body& inBody2,
                const JPH::ContactManifold&,
                JPH::ContactSettings&
            ) override
            {
                push(inBody1, inBody2);
            }

            void push(const JPH::Body& inBody1, const JPH::Body& inBody2)
            {
                const std::lock_guard<std::mutex> lock(mutex);
                pairs.push_back(
                    {inBody1.GetID().GetIndexAndSequenceNumber(), inBody2.GetID().GetIndexAndSequenceNumber()}
                );
            }

        public:
            std::mutex                 mutex;
            std::vector<ContactRecord> pairs;
        };

        struct Engine::Implementation
        {
        public:
            Implementation();

        public:
            void applyGravity(const Vec3& inValue)
            {
                gravity = inValue;

                system.SetGravity(Convert::toPhysicsPosition(gravity));
            }

            JPH::BodyInterface& bodies() { return system.GetBodyInterface(); }

            const JPH::BodyInterface& bodies() const { return system.GetBodyInterface(); }

            JPH::RefConst<JPH::Shape> createShape(const BodyCreateInfo& inCreateInfo);
            void applyPendingWrites();

        public:
            JPH::JobSystemThreadPool                                   threadPool;
            JPH::TempAllocatorImpl                                     tempAllocator;
            JPH::BroadPhaseLayerInterfaceTable                         broadLayer;
            JPH::ObjectLayerPairFilterTable                            objectLayer;
            std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilterTable>   objectVsBroadPhaseLayer;
            JPH::RefConst<JPH::GroupFilterTable>                       groupFilter;
            std::unique_ptr<ContactRelay>                              contacts;
            JPH::PhysicsSystem                                         system;
            std::unordered_map<Hash::Value, JPH::RefConst<JPH::Shape>> shapes;
            std::vector<JPH::BodyID>                                   ids;
            std::unordered_map<std::uint32_t, void*>                   owners;
            Mailbox<PendingWrite>                                      pending;
            std::unordered_map<std::uint32_t, JPH::Vec3>               horizontalWish;
            std::unordered_map<std::uint32_t, JPH::RVec3>              previousPosition;
            Vec3                                                       gravity;
            bool                                                       bBroadPhaseDirty = false;
            float                                                      accumulator;
        };

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

        static JPH::BodyID toId(Body inBody)
        {
            return JPH::BodyID(inBody.value());
        }

        static Body toBody(JPH::BodyID inId)
        {
            return Body(inId.GetIndexAndSequenceNumber());
        }

        static JPH::ObjectLayer toPhysicsObjectLayer(ObjectLayer inLayer)
        {
            return static_cast<JPH::ObjectLayer>(inLayer);
        }

        static bool isNonMovingLayer(ObjectLayer inLayer)
        {
            return inLayer == ObjectLayer::NonMoving || inLayer == ObjectLayer::NoCollision;
        }

        static ObjectLayer resolveCreateLayer(const BodyCreateInfo& inCreateInfo)
        {
            if (inCreateInfo.bUsePreset)
            {
                const ObjectLayer layer = resolveCollisionPreset(inCreateInfo.preset, inCreateInfo.motion).layer;
                if (layer != ObjectLayer::Auto)
                {
                    return layer;
                }
            }

            return resolveObjectLayer(inCreateInfo.layer, inCreateInfo.motion);
        }

        static bool resolveCreateSensor(const BodyCreateInfo& inCreateInfo)
        {
            if (inCreateInfo.bUsePreset)
            {
                const CollisionPresetInfo info = resolveCollisionPreset(inCreateInfo.preset, inCreateInfo.motion);
                if (info.layer != ObjectLayer::Auto)
                {
                    return info.bIsSensor || inCreateInfo.bIsSensor;
                }
            }

            return inCreateInfo.bIsSensor || inCreateInfo.layer == ObjectLayer::Trigger;
        }

        static float resolveCreateMass(const BodyCreateInfo& inCreateInfo)
        {
            return std::max(0.1f, inCreateInfo.mass * std::max(0.0f, inCreateInfo.massScale));
        }

        static void enableLayerPair(JPH::ObjectLayerPairFilterTable& inTable, ObjectLayer inA, ObjectLayer inB)
        {
            const JPH::ObjectLayer a = toPhysicsObjectLayer(inA);
            const JPH::ObjectLayer b = toPhysicsObjectLayer(inB);
            inTable.EnableCollision(a, b);
            if (a != b)
            {
                inTable.EnableCollision(b, a);
            }
        }

        static void disableLayerPair(JPH::ObjectLayerPairFilterTable& inTable, ObjectLayer inA, ObjectLayer inB)
        {
            inTable.DisableCollision(toPhysicsObjectLayer(inA), toPhysicsObjectLayer(inB));
        }

        static void setupObjectLayerMatrix(JPH::ObjectLayerPairFilterTable& inTable)
        {
            for (std::uint32_t left = 0; left < OBJECT_LAYER_COUNT; ++left)
            {
                for (std::uint32_t right = left; right < OBJECT_LAYER_COUNT; ++right)
                {
                    const ObjectLayer a = static_cast<ObjectLayer>(left);
                    const ObjectLayer b = static_cast<ObjectLayer>(right);
                    if (canCollide(a, b))
                    {
                        enableLayerPair(inTable, a, b);
                    }
                }
            }
        }

        static Hash::Value hashPolygon(const BodyPolygon& inPolygon)
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

        Engine::Implementation::Implementation()
            : tempAllocator(10 * 1024 * 1024),
              broadLayer(OBJECT_LAYER_COUNT, BroadPhaseLayer::Count),
              objectLayer(OBJECT_LAYER_COUNT),
              groupFilter(new JPH::GroupFilterTable(MAX_COLLISION_SUB_GROUPS)),
              gravity(sGetEarthGravity()),
              accumulator(0.0f)
        {
            threadPool.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, -1);

            for (std::uint32_t i = 0; i < OBJECT_LAYER_COUNT; i++)
            {
                const ObjectLayer layer = static_cast<ObjectLayer>(i);
                broadLayer.MapObjectToBroadPhaseLayer(
                    toPhysicsObjectLayer(layer),
                    isNonMovingLayer(layer) ? BroadPhaseLayer::NonMoving : BroadPhaseLayer::Moving
                );
            }

            setupObjectLayerMatrix(objectLayer);

            // Custom layers stay reachable in both broadphase trees. The pair table is the real filter
            // and can change after startup.
            std::vector<std::pair<ObjectLayer, ObjectLayer>> temporaryPairs;
            for (std::uint8_t index = static_cast<std::uint8_t>(ObjectLayer::Custom_0);
                 index < static_cast<std::uint8_t>(ObjectLayer::Count);
                 ++index)
            {
                const ObjectLayer layer = static_cast<ObjectLayer>(index);
                if (!canCollide(layer, ObjectLayer::NonMoving))
                {
                    enableLayerPair(objectLayer, layer, ObjectLayer::NonMoving);
                    temporaryPairs.emplace_back(layer, ObjectLayer::NonMoving);
                }

                if (!canCollide(layer, ObjectLayer::Moving))
                {
                    enableLayerPair(objectLayer, layer, ObjectLayer::Moving);
                    temporaryPairs.emplace_back(layer, ObjectLayer::Moving);
                }
            }

            objectVsBroadPhaseLayer = std::make_unique<JPH::ObjectVsBroadPhaseLayerFilterTable>(
                broadLayer,
                BroadPhaseLayer::Count,
                objectLayer,
                OBJECT_LAYER_COUNT
            );

            for (const std::pair<ObjectLayer, ObjectLayer>& pair : temporaryPairs)
            {
                disableLayerPair(objectLayer, pair.first, pair.second);
            }

            contacts = std::make_unique<ContactRelay>();
            system.Init(
                MAX_BODIES,
                MAX_BODY_MUTEXES,
                MAX_BODY_PAIRS,
                MAX_CONTACT_CONSTRAINTS,
                broadLayer,
                *objectVsBroadPhaseLayer,
                objectLayer
            );
            system.SetContactListener(contacts.get());
            system.SetGravity(Convert::toPhysicsPosition(gravity));
        }

        static JPH::RefConst<JPH::Shape> makeBox(const Vec3& inSize)
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

        static JPH::RefConst<JPH::Shape> makeCapsule(const Vec3& inSize)
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

        static JPH::RefConst<JPH::Shape> makeHull(const BodyPolygon& inPolygon)
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

        Engine& Engine::sInstance()
        {
            static Engine instance;

            return instance;
        }

        const Vec3& Engine::sGetZeroGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Zero);

            return value;
        }

        const Vec3& Engine::sGetMercuryGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Mercury);

            return value;
        }

        const Vec3& Engine::sGetVenusGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Venus);

            return value;
        }

        const Vec3& Engine::sGetEarthGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Earth);

            return value;
        }

        const Vec3& Engine::sGetMoonGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Moon);

            return value;
        }

        const Vec3& Engine::sGetMarsGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Mars);

            return value;
        }

        const Vec3& Engine::sGetJupiterGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Jupiter);

            return value;
        }

        const Vec3& Engine::sGetSaturnGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Saturn);

            return value;
        }

        const Vec3& Engine::sGetUranusGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Uranus);

            return value;
        }

        const Vec3& Engine::sGetNeptuneGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Neptune);

            return value;
        }

        const Vec3& Engine::sGetPlutoGravity()
        {
            static const Vec3 value = Gravity::sDown(Gravity::Pluto);

            return value;
        }

        const Vec3& Engine::sGetPlanetGravity(Planet inPlanet)
        {
            switch (inPlanet)
            {
            case Planet::Mercury:
                return sGetMercuryGravity();

            case Planet::Venus:
                return sGetVenusGravity();

            case Planet::Moon:
                return sGetMoonGravity();

            case Planet::Mars:
                return sGetMarsGravity();

            case Planet::Jupiter:
                return sGetJupiterGravity();

            case Planet::Saturn:
                return sGetSaturnGravity();

            case Planet::Uranus:
                return sGetUranusGravity();

            case Planet::Neptune:
                return sGetNeptuneGravity();

            case Planet::Pluto:
                return sGetPlutoGravity();

            case Planet::Earth:
            default:
                return sGetEarthGravity();
            }
        }

        void Engine::sSetZeroGravity()
        {
            sSetGravity(sGetZeroGravity());
        }

        void Engine::sSetGravity(Planet inPlanet)
        {
            sSetGravity(sGetPlanetGravity(inPlanet));
        }

        void Engine::sSetGravity(const Vec3& inValue)
        {
            Engine& engine = sInstance();
            if (!engine.m_implementation)
            {
                return;
            }

            engine.m_implementation->applyGravity(inValue);
        }

        void Engine::applyObjectLayerPair(ObjectLayer inA, ObjectLayer inB, bool bInCollide)
        {
            if (!m_implementation)
            {
                return;
            }

            if (bInCollide)
            {
                enableLayerPair(m_implementation->objectLayer, inA, inB);

                return;
            }

            disableLayerPair(m_implementation->objectLayer, inA, inB);
        }

        Engine::Engine()
            : m_implementation(std::make_unique<Implementation>())
        {
            setObjectLayerPairCallback(
                [this](ObjectLayer inA, ObjectLayer inB, bool bInCollide)
                {
                    applyObjectLayerPair(inA, inB, bInCollide);
                }
            );
        }

        Engine::~Engine()
        {
            setObjectLayerPairCallback({});

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
                return sGetEarthGravity();
            }

            return m_implementation->gravity;
        }

        void Engine::Implementation::applyPendingWrites()
        {
            CH_TRACER_SCOPE_NAMED("Physics Writes");

            const std::vector<PendingWrite> writes = pending.drain();

            JPH::BodyInterface& interface = bodies();
            for (const PendingWrite& write : writes)
            {
                if (write.id.IsInvalid() || !interface.IsAdded(write.id))
                {
                    continue;
                }

                if (write.kind == PendingWriteKind::Impulse)
                {
                    interface.AddImpulse(write.id, write.vector, write.location);

                    continue;
                }

                if (write.kind == PendingWriteKind::HorizontalVelocity)
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

            CH_TRACER_SCOPE_NAMED("Physics");

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

                {
                    CH_TRACER_SCOPE_NAMED("Physics Step");

                    m_implementation->system.Update(
                        FIXED_STEP,
                        COLLISION_STEPS,
                        &m_implementation->tempAllocator,
                        &m_implementation->threadPool
                    );
                }
                m_implementation->accumulator -= FIXED_STEP;

                steps++;
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
                return Body::sInvalid();
            }

            const JPH::RefConst<JPH::Shape> shape = m_implementation->createShape(inCreateInfo);
            if (shape == nullptr)
            {
                return Body::sInvalid();
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
            settings.mAllowDynamicOrKinematic      = true;
            settings.mCollideKinematicVsNonDynamic = inCreateInfo.motion == MotionType::Kinematic;
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

            const bool bShapeCapsuleAndNotStatic =
                static_cast<bool>(inCreateInfo.shape == BodyShape::Capsule && !bIsStatic);

            if (bShapeCapsuleAndNotStatic)
            {
                settings.mFriction       = 0.0f;
                settings.mRestitution    = 0.0f;
                settings.mLinearDamping  = 0.0f;
                settings.mAngularDamping = 0.0f;
                settings.mMotionQuality  = JPH::EMotionQuality::LinearCast;
                settings.mAllowedDOFs =
                    JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ;
            }

            if (!bShapeCapsuleAndNotStatic)
            {
                settings.mMotionQuality = bIsStatic ? JPH::EMotionQuality::Discrete : JPH::EMotionQuality::LinearCast;
            }

            JPH::Body* created = m_implementation->bodies().CreateBody(settings);
            if (!created)
            {
                return Body::sInvalid();
            }

            const JPH::BodyID id = created->GetID();
            m_implementation->ids.push_back(id);

            return toBody(id);
        }

        void Engine::setBodyOwner(Body inBody, void* inOwner)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            const std::uint32_t key = id.GetIndexAndSequenceNumber();
            if (!inOwner)
            {
                m_implementation->owners.erase(key);

                return;
            }

            m_implementation->owners[key] = inOwner;
        }

        void* Engine::getBodyOwner(Body inBody) const
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return nullptr;
            }

            const auto found = m_implementation->owners.find(id.GetIndexAndSequenceNumber());
            if (found == m_implementation->owners.end())
            {
                return nullptr;
            }

            return found->second;
        }

        std::vector<BodyContact> Engine::drainBodyContacts()
        {
            std::vector<BodyContact> result;
            if (!m_implementation || !m_implementation->contacts)
            {
                return result;
            }

            std::vector<ContactRecord> records;
            {
                const std::lock_guard<std::mutex> lock(m_implementation->contacts->mutex);
                records.swap(m_implementation->contacts->pairs);
            }

            result.reserve(records.size());
            for (const ContactRecord& record : records)
            {
                BodyContact contact;
                contact.first  = Body(record.first);
                contact.second = Body(record.second);

                result.push_back(contact);
            }

            return result;
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
            m_implementation->owners.erase(id.GetIndexAndSequenceNumber());
            m_implementation->horizontalWish.erase(id.GetIndexAndSequenceNumber());
            m_implementation->previousPosition.erase(id.GetIndexAndSequenceNumber());

            std::vector<JPH::BodyID>& ids = m_implementation->ids;
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

            m_implementation->pending.push(
                {PendingWriteKind::Impulse,
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
                return Vec3::sZero();
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

            m_implementation->pending.push(
                {PendingWriteKind::Velocity, id, Convert::toPhysicsPosition(inVelocity), JPH::Vec3::sZero()}
            );
        }

        void Engine::setBodyHorizontalVelocity(Body inBody, const Vec3& inVelocity)
        {
            const JPH::BodyID id = toId(inBody);
            if (id.IsInvalid() || !m_implementation)
            {
                return;
            }

            m_implementation->pending.push(
                {PendingWriteKind::HorizontalVelocity,
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

                for (int i = 0; i < triangleCount; i++)
                {
                    const std::uint32_t offset = static_cast<std::uint32_t>(result.second.size());

                    for (int j = 0; j < 3; j++)
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
