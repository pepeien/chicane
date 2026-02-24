#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Chicane/Core/Hash.hpp"
#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Bounds/3D.hpp"

#include "Chicane/Kerb.hpp"
#include "Chicane/Kerb/Body/CreateInfo.hpp"

namespace Chicane
{
    namespace Kerb
    {
        class CHICANE_KERB Engine
        {
            friend class Application;

        public:
            static inline Engine& getInstance()
            {
                static Engine instance;

                return instance;
            }

        public:
            Engine(const Engine&)            = delete;
            Engine& operator=(const Engine&) = delete;

            Engine(Engine&&)            = delete;
            Engine& operator=(Engine&&) = delete;

        private:
            Engine();
            ~Engine();

        public:
            // Lifetime
            void tick(float inDeltaTime);

            // Body creation
            JPH::BodyID createBody(const BodyCreateInfo& inCreateInfo);
            void activateBody(JPH::BodyID inId);
            void deactivateBody(JPH::BodyID inId);
            void destroyBody(JPH::BodyID inId);

            // Body settings
            void setBodyMotion(JPH::BodyID inId, MotionType inType);

            // Body movement
            void addBodyImpulse(JPH::BodyID inId, const Vec3& inDirection, float inForce, const Vec3& inLocation);

            Transform getBodyTransform(JPH::BodyID inId);
            void setBodyTransform(JPH::BodyID inId, const Transform& inValue);

            // Body drawing
            std::pair<Vertex::Indices, Vertex::List> getBodyPolygon(JPH::BodyID inId) const;

        private:
            // Body creation
            JPH::BodyID createDynamicBody(const BodyCreateInfo& inCreateInfo);
            JPH::BodyID createKinematicBody(const BodyCreateInfo& inCreateInfo);
            JPH::BodyID createStaticBody(const BodyCreateInfo& inCreateInfo);

            JPH::Shape* generateShape(BodyShape inType, const Bounds3D& inBounds) const;

            // Body settings
            JPH::EMotionType toPhysicsMotionType(MotionType inValue) const;

            // Body movement
            JPH::Vec3 toPhysicsPosition(Vec3 inValue) const;
            JPH::Vec3 toPhysicsSize(Vec3 inValue) const;
            JPH::Quat toPhysicsRotation(QuatFloat inValue) const;

            Vec3 toEnginePosition(JPH::Vec3 inValue) const;
            QuatFloat toEngineRotation(JPH::Quat inValue) const;

            // Cache
            Hash::Value hashShape(const Vertex::Positions& inPositions, const Vertex::Indices& inIndices);

        private:
            JPH::JobSystemThreadPool                                 m_threadPool;
            JPH::TempAllocatorImpl                                   m_tempAllocator;
            JPH::BroadPhaseLayerInterfaceTable                       m_broadLayer;
            JPH::ObjectLayerPairFilterTable                          m_objectLayer;
            std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilterTable> m_objectVsBroadPhaseLayer;
            JPH::PhysicsSystem                                       m_system;

            std::unordered_map<Hash::Value, JPH::Ref<JPH::Shape>>    m_shapes;
            JPH::BodyIDVector                                        m_bodies;
        };
    }
}