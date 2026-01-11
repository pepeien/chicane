#pragma once

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Kerb.hpp"

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace Chicane
{
    namespace Kerb
    {
        class CHICANE_KERB Engine
        {
        public:
            static Engine& getInstance()
            {
                static Engine instance;

                return instance;
            }

        public:
            Engine(const Engine&)            = delete;
            Engine& operator=(const Engine&) = delete;

            Engine(Engine&&)                 = delete;
            Engine& operator=(Engine&&)      = delete;

        public:
            JPH::Body* createBodyFromMesh(
                const std::vector<Vertex>& inVertices, const std::vector<std::uint32_t>& inIndices
            );

        private:
            Engine();

            ~Engine();

        private:
            JPH::JobSystemThreadPool                m_threadPool;
            JPH::BroadPhaseLayerInterfaceTable      m_broadLayer;
            JPH::ObjectLayerPairFilterTable         m_objectLayer;
            JPH::ObjectVsBroadPhaseLayerFilterTable m_objectVsBroadPhaseLayer;
            JPH::PhysicsSystem                      m_system;
        };
    }
}