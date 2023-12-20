#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Vertex.hpp"

namespace Engine
{
    namespace Mesh
    {
        namespace Manager
        {
            class Instance
            {
            public:
                void addMesh(
                    const std::string& inMeshId,
                    const std::vector<Vertex::Instance>& inVertices
                );
                void drawMesh(
                    const std::string& inMeshId,
                    const vk::CommandBuffer& inCommadBuffer
                );

                void initVertexBuffer(
                    Buffer::Instance& outVertexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

            private:
                void extractFromMeshList(
                    std::vector<Vertex::Instance>& outVertices,
                    vk::DeviceSize& outAllocationSize
                );

            private:
                std::vector<std::string> meshesOrder;
                std::unordered_map<std::string, Mesh::Instance> meshesMap;
            };
        }
    }
}