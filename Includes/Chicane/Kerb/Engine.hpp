#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Kerb.hpp"

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
                const Vertex::List& inVertices, const Vertex::Indices& inIndices
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