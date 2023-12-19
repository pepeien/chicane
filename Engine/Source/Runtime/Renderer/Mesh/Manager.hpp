#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/Vertex/Instance.hpp"
#include "Renderer/Vertex/Buffer.hpp"

namespace Engine
{
    namespace Mesh
    {
        namespace Manager
        {
            class Instance
            {
            public:
                void addMesh(const std::string& inMeshId, const std::vector<Vertex::Base*>& inVertices);
                Mesh::Instance getMesh(const std::string& inMeshId);

                void builMeshes(
                    Vertex::Buffer::Instance& outVertexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

            private:
                void extractFromMeshList(std::vector<Vertex::Base>& outVertices, vk::DeviceSize& outAllocationSize);

            private:
                std::vector<std::string> meshesOrder;
                std::unordered_map<std::string, Mesh::Instance> meshesMap;
            };
        }
    }
}