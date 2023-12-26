#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Wavefront.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Vertex.hpp"

namespace Chicane
{
    namespace Mesh
    {
        enum class Type
        {
            // N/A
            Undefined,
            // .obj
            Wavefront
        };

        namespace Manager
        {
            class Instance
            {
            public:
                void importMesh(
                    const std::string& inMeshId,
                    const std::string& inFilePath,
                    Type inType
                );
                void drawMesh(
                    const std::string& inId,
                    uint32_t inInstanceCount,
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

                void addMesh(
                    const std::string& inMeshId,
                    const std::vector<Vertex::Instance>& inVertices,
                    const std::vector<uint32_t>& inIndexes
                );

                void combineVertices();

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
                std::vector<Vertex::Instance> m_combinedVertices;
                std::vector<uint32_t> m_indexedVertices;

                std::vector<std::string> m_registeredMeshIds;
                std::unordered_map<std::string, Mesh::AllocationInfo> m_meshAllocationInfos;
                std::unordered_map<std::string, Mesh::Instance> m_meshInstances;
            };
        }
    }
}