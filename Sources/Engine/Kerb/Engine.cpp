#include "Chicane/Kerb/Engine.hpp"

#include "Chicane/Core/Log.hpp"
#include "Chicane/Kerb/Layer/BroadPhase.hpp"
#include "Chicane/Kerb/Layer/Object.hpp"

#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/PhysicsSettings.h>

namespace Chicane
{
    namespace Kerb
    {
        static constexpr std::uint32_t MAX_BODIES              = 65536;
        static constexpr std::uint32_t MAX_BODY_MUTEXES        = 0;
        static constexpr std::uint32_t MAX_BODY_PAIRS          = MAX_BODIES;
        static constexpr std::uint32_t MAX_CONTACT_CONSTRAINTS = 10240;

        Engine::Engine()
            : m_threadPool({}),
              m_broadLayer(ObjectLayer::Count, BroadPhaseLayer::Count),
              m_objectLayer(ObjectLayer::Count),
              m_objectVsBroadPhaseLayer(m_broadLayer, BroadPhaseLayer::Count, m_objectLayer, ObjectLayer::Count),
              m_system({})
        {
            m_threadPool.Init(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
            m_system.Init(
                MAX_BODIES,
                MAX_BODY_MUTEXES,
                MAX_BODY_PAIRS,
                MAX_CONTACT_CONSTRAINTS,
                m_broadLayer,
                m_objectVsBroadPhaseLayer,
                m_objectLayer
            );
        }

        Engine::~Engine()
        {
            exit();
        }

        JPH::Body* Engine::createBodyFromMesh(
            const std::vector<Vertex>& inVertices, const std::vector<std::uint32_t>& inIndices
        )
        {
            JPH::VertexList vertices;
            vertices.resize(inVertices.size());

            for (std::uint32_t i = 0; i < inVertices.size(); i++)
            {
                const Vertex& vertex = inVertices.at(i);

                vertices[i] = {vertex.position.x, vertex.position.y, vertex.position.z};
            }

            JPH::IndexedTriangleList indices;
            indices.resize(inIndices.size() / 3);

            for (std::uint32_t i = 0; i < inIndices.size(); i += 3)
            {
                indices[i / 3] = JPH::IndexedTriangle(inIndices.at(i), inIndices.at(i + 1), inIndices.at(i + 2));
            }

            JPH::MeshShapeSettings shapeSettings = JPH::MeshShapeSettings(vertices, indices);
            shapeSettings.SetEmbedded();

            JPH::BodyCreationSettings bodySettings = JPH::BodyCreationSettings(
                &shapeSettings,
                JPH::Vec3::sZero(),
                JPH::Quat::sZero(),
                JPH::EMotionType::Static,
                ObjectLayer::NonMoving
            );

            return m_system.GetBodyInterface().CreateBody(bodySettings);
        }
    }
}