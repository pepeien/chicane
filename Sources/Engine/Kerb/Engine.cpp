#include "Chicane/Kerb/Engine.hpp"

#include "Chicane/Core/Log.hpp"

#include <Jolt/Core/Factory.h>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>

namespace Chicane
{
    namespace Kerb
    {
        Engine::Engine()
        {
            JPH::RegisterDefaultAllocator();

            JPH::Factory::sInstance = new JPH::Factory();

            JPH::RegisterTypes();
        }

        Engine::~Engine()
        {
            JPH::UnregisterTypes();

            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }

        JPH::ShapeSettings::ShapeResult Engine::createShapeFromMesh(
            const std::vector<Vertex>& inVertices, const std::vector<std::uint32_t>& inIndices
        ) const
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

            JPH::MeshShapeSettings settings(vertices, indices);
            settings.SetEmbedded();

            return settings.Create();
        }
    }
}