#include "Chicane/Kerb/Engine.hpp"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/PhysicsSettings.h>

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
        namespace ObjectLayer
        {
            static constexpr JPH::ObjectLayer NonMoving = 0;
            static constexpr JPH::ObjectLayer Moving    = 1;

            static constexpr std::uint32_t Count = 2;
        }

        static constexpr std::uint32_t MAX_BODIES              = 65536;
        static constexpr std::uint32_t MAX_BODY_MUTEXES        = 0;
        static constexpr std::uint32_t MAX_BODY_PAIRS          = MAX_BODIES;
        static constexpr std::uint32_t MAX_CONTACT_CONSTRAINTS = 10240;

        static const JPH::Quat ENGINE_TO_JPH_QUAT = JPH::Quat::sRotation(JPH::Vec3::sAxisX(), JPH::JPH_PI * 0.5f);
        static const JPH::Quat JPH_TO_ENGINE_QUAT = JPH::Quat::sRotation(JPH::Vec3::sAxisX(), -JPH::JPH_PI * 0.5f);

        Engine::Engine()
            : m_tempAllocator(10 * 1024 * 1024),
              m_broadLayer(ObjectLayer::Count, BroadPhaseLayer::Count),
              m_objectLayer(ObjectLayer::Count),
              m_shapes({}),
              m_bodies({})
        {
            m_threadPool.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

            m_broadLayer.MapObjectToBroadPhaseLayer(ObjectLayer::NonMoving, BroadPhaseLayer::NonMoving);
            m_broadLayer.MapObjectToBroadPhaseLayer(ObjectLayer::Moving, BroadPhaseLayer::Moving);

            m_objectLayer.EnableCollision(ObjectLayer::NonMoving, ObjectLayer::Moving);
            m_objectLayer.EnableCollision(ObjectLayer::Moving, ObjectLayer::Moving);
            m_objectLayer.EnableCollision(ObjectLayer::Moving, ObjectLayer::NonMoving);

            m_objectVsBroadPhaseLayer = std::make_unique<JPH::ObjectVsBroadPhaseLayerFilterTable>(
                m_broadLayer,
                BroadPhaseLayer::Count,
                m_objectLayer,
                ObjectLayer::Count
            );

            m_system.Init(
                MAX_BODIES,
                MAX_BODY_MUTEXES,
                MAX_BODY_PAIRS,
                MAX_CONTACT_CONSTRAINTS,
                m_broadLayer,
                *m_objectVsBroadPhaseLayer,
                m_objectLayer
            );
            m_system.SetGravity(JPH::Vec3(0.0f, -3.6f, 0.0f));
        }

        Engine::~Engine()
        {
            shutdown();
        }

        void Engine::tick(float inDeltaTime)
        {
            m_system.Update(1 / 120.0f, 8, &m_tempAllocator, &m_threadPool);
        }

        void Engine::addBodyImpulse(JPH::BodyID inId, const Vec3& inDirection, float inForce, const Vec3& inLocation)
        {
            if (inId.IsInvalid())
            {
                return;
            }

            m_system.GetBodyInterface()
                .AddImpulse(inId, toPhysicsPosition(inDirection * inForce), toPhysicsPosition(inLocation));
        }

        Transform Engine::getBodyTransform(JPH::BodyID inId)
        {
            if (inId.IsInvalid())
            {
                return {};
            }

            const JPH::Mat44 transform = m_system.GetBodyInterface().GetWorldTransform(inId);

            Transform result;
            result.setTranslation(toEnginePosition(transform.GetTranslation()));
            result.setRotation(Rotator(toEngineRotation(transform.GetRotation().GetQuaternion())));

            return result;
        }

        void Engine::setBodyTransform(JPH::BodyID inId, const Transform& inValue)
        {
            if (inId.IsInvalid())
            {
                return;
            }

            m_system.GetBodyInterface().SetPositionAndRotation(
                inId,
                toPhysicsPosition(inValue.getTranslation()),
                toPhysicsRotation(inValue.getRotation().get()),
                JPH::EActivation::Activate
            );
        }

        JPH::BodyID Engine::createBody(const BodyCreateInfo& inCreateInfo)
        {
            switch (inCreateInfo.motion)
            {
            case MotionType::Dynamic:
                return createDynamicBody(inCreateInfo);

            case MotionType::Kinematic:
                return createDynamicBody(inCreateInfo);

            case MotionType::Static:
                return createStaticBody(inCreateInfo);

            default:
                return JPH::BodyID(JPH::BodyID::cInvalidBodyID);
            }
        }

        void Engine::activateBody(JPH::BodyID inId)
        {
            if (inId.IsInvalid())
            {
                return;
            }

            m_system.GetBodyInterface().AddBody(inId, JPH::EActivation::DontActivate);
        }

        void Engine::deactivateBody(JPH::BodyID inId)
        {
            if (inId.IsInvalid())
            {
                return;
            }

            m_system.GetBodyInterface().RemoveBody(inId);
        }

        void Engine::destroyBody(JPH::BodyID inId)
        {
            if (inId.IsInvalid())
            {
                return;
            }

            m_system.GetBodyInterface().RemoveBody(inId);
            m_system.GetBodyInterface().DestroyBody(inId);
        }

        void Engine::setBodyMotion(JPH::BodyID inId, MotionType inType)
        {
            if (inId.IsInvalid())
            {
                return;
            }

            m_system.GetBodyInterface().SetMotionType(inId, toPhysicsMotionType(inType), JPH::EActivation::Activate);
        }

        std::pair<Vertex::Indices, Vertex::List> Engine::getBodyPolygon(JPH::BodyID inId) const
        {
            if (inId.IsInvalid())
            {
                return {};
            }

            const JPH::TransformedShape shape = m_system.GetBodyInterface().GetTransformedShape(inId);

            JPH::Shape::GetTrianglesContext context;
            shape.GetTrianglesStart(context, shape.GetWorldSpaceBounds(), JPH::Vec3::sReplicate(0.0f));

            const int   stepSize = 256;
            JPH::Float3 vertices[stepSize * 3];

            std::pair<Vertex::Indices, Vertex::List> result;

            while (true)
            {
                const int vertexCount = shape.GetTrianglesNext(context, stepSize, vertices);

                if (vertexCount <= 0)
                {
                    break;
                }

                for (int i = 0; i < vertexCount; i++)
                {
                    std::uint32_t offset = static_cast<std::uint32_t>(result.second.size());

                    for (int j = 0; j < 3; j++)
                    {
                        result.first.push_back(offset + j);

                        Vertex vertex;
                        vertex.position = {vertices[i * 3 + j].x, vertices[i * 3 + j].y, vertices[i * 3 + j].z};
                        vertex.color    = Vec4(255.0f);

                        result.second.push_back(vertex);
                    }
                }
            }

            return result;
        }

        JPH::BodyID Engine::createDynamicBody(const BodyCreateInfo& inCreateInfo)
        {
            JPH::BodyCreationSettings settings(
                generateShape(inCreateInfo.shape, inCreateInfo.bounds),
                toPhysicsPosition(inCreateInfo.bounds.getCenter()),
                JPH::Quat::sIdentity(),
                JPH::EMotionType::Dynamic,
                ObjectLayer::Moving
            );
            // Runtime
            settings.mAllowSleeping = false;

            // Gravity
            settings.mGravityFactor                = 1.0f;
            settings.mMotionQuality                = JPH::EMotionQuality::Discrete;
            settings.mOverrideMassProperties       = JPH::EOverrideMassProperties::CalculateInertia;
            settings.mMassPropertiesOverride.mMass = inCreateInfo.mass;

            JPH::BodyID id = m_system.GetBodyInterface().CreateAndAddBody(settings, JPH::EActivation::DontActivate);
            m_bodies.push_back(id);

            return id;
        }

        JPH::BodyID Engine::createKinematicBody(const BodyCreateInfo& inCreateInfo)
        {
            JPH::BodyCreationSettings settings(
                generateShape(inCreateInfo.shape, inCreateInfo.bounds),
                toPhysicsPosition(inCreateInfo.bounds.getCenter()),
                JPH::Quat::sIdentity(),
                JPH::EMotionType::Kinematic,
                ObjectLayer::Moving
            );
            // Runtime
            settings.mAllowSleeping = false;

            // Gravity
            settings.mGravityFactor                = 1.0f;
            settings.mMotionQuality                = JPH::EMotionQuality::Discrete;
            settings.mOverrideMassProperties       = JPH::EOverrideMassProperties::CalculateInertia;
            settings.mMassPropertiesOverride.mMass = inCreateInfo.mass;

            JPH::BodyID id = m_system.GetBodyInterface().CreateAndAddBody(settings, JPH::EActivation::DontActivate);
            m_bodies.push_back(id);

            return id;
        }

        JPH::BodyID Engine::createStaticBody(const BodyCreateInfo& inCreateInfo)
        {
            JPH::BodyCreationSettings settings(
                generateShape(inCreateInfo.shape, inCreateInfo.bounds),
                toPhysicsPosition(inCreateInfo.bounds.getCenter()),
                JPH::Quat::sIdentity(),
                JPH::EMotionType::Static,
                ObjectLayer::NonMoving
            );
            // Runtime
            settings.mAllowDynamicOrKinematic = true;
            settings.mAllowSleeping           = false;

            // Gravity
            settings.mGravityFactor                = 1.0f;
            settings.mMotionQuality                = JPH::EMotionQuality::Discrete;
            settings.mOverrideMassProperties       = JPH::EOverrideMassProperties::CalculateInertia;
            settings.mMassPropertiesOverride.mMass = inCreateInfo.mass;

            JPH::BodyID id = m_system.GetBodyInterface().CreateAndAddBody(settings, JPH::EActivation::DontActivate);
            m_bodies.push_back(id);

            return id;
        }

        JPH::Shape* Engine::generateShape(BodyShape inType, const Bounds3D& inBounds) const
        {
            switch (inType)
            {
            case BodyShape::Capsule:
                return new JPH::CapsuleShape(inBounds.getSize().z * 0.5f, inBounds.getSize().x);

            default:
                return new JPH::BoxShape(toPhysicsSize(inBounds.getSize() * 0.5f), 0.05f);
            }
        }

        JPH::EMotionType Engine::toPhysicsMotionType(MotionType inValue) const
        {
            switch (inValue)
            {
            case MotionType::Dynamic:
                return JPH::EMotionType::Dynamic;

            case MotionType::Kinematic:
                return JPH::EMotionType::Kinematic;

            default:
                return JPH::EMotionType::Static;
            }
        }

        JPH::Vec3 Engine::toPhysicsPosition(Vec3 inValue) const
        {
            return JPH::Vec3(inValue.x, inValue.z, -inValue.y);
        }

        JPH::Vec3 Engine::toPhysicsSize(Vec3 inValue) const
        {
            return JPH::Vec3(inValue.x, inValue.z, inValue.y);
        }

        JPH::Quat Engine::toPhysicsRotation(QuatFloat inValue) const
        {
            return JPH::Quat(inValue.x, inValue.y, inValue.z, inValue.w);
        }

        Vec3 Engine::toEnginePosition(JPH::Vec3 inValue) const
        {
            return Vec3(inValue.GetX(), -inValue.GetZ(), inValue.GetY());
        }

        QuatFloat Engine::toEngineRotation(JPH::Quat inValue) const
        {
            JPH::Quat converted = inValue * JPH_TO_ENGINE_QUAT;
            return QuatFloat(converted.GetX(), converted.GetY(), converted.GetZ(), converted.GetW());
        }

        Hash::Value Engine::hashShape(const Vertex::Positions& inPositions, const Vertex::Indices& inIndices)
        {
            Hash::Subject data;
            data.data = inIndices.data();
            data.size = inIndices.size() * sizeof(Vertex::Index);

            Hash::Subject seed;
            seed.data = inPositions.data();
            seed.size = inPositions.size() * sizeof(Vertex::Position);

            return Hash::generate(data, seed);
        }
    }
}