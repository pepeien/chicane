#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Vertex.hpp"

namespace Chicane
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
                    const std::string& inId,
                    const uint32_t& inInstanceCount,
                    const vk::CommandBuffer& inCommadBuffer
                );

                void initBuffers(
                    Buffer::Instance& outVertexBuffer,
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

            private:
                void setup();

                void initVertexBuffer(
                    Buffer::Instance& outVertexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );
                void initIndexBuffer(
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

            private:
                std::vector<Vertex::Instance> combinedVertices;
                std::vector<uint32_t> indexedVertices;

                std::vector<std::string> registeredMeshIds;
                std::unordered_map<std::string, Mesh::AllocationInfo> meshAllocationInfos;
                std::unordered_map<std::string, Mesh::Instance> meshInstances;
            };
        }
    }
}