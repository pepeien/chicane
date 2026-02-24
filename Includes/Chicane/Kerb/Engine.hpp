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
            void tick(float inDeltaTime);

            void addImpulse(JPH::BodyID inId, const Vec3& inDirection, float inForce, const Vec3& inLocation);

            Transform getTransform(JPH::BodyID inId);
            void setTransform(JPH::BodyID inId, const Transform& inValue);

            JPH::BodyID createStaticBody(
                const Vec3& inTranslation, const Rotator& inRotation, const Bounds3D& inBounds
            );
            JPH::BodyID createDynamicBody(
                const Vec3& inTranslation, const Rotator& inRotation, const Bounds3D& inBounds
            );

            std::pair<Vertex::Indices, Vertex::List> getBodyPolygon(JPH::BodyID inId) const;

        protected:
            JPH::Vec3 toPhysicsPosition(Vec3 inValue) const;
            JPH::Vec3 toPhysicsSize(Vec3 inValue) const;
            JPH::Quat toPhysicsRotation(QuatFloat inValue) const;

            Vec3 toEnginePosition(JPH::Vec3 inValue) const;
            QuatFloat toEngineRotation(JPH::Quat inValue) const;

        private:
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