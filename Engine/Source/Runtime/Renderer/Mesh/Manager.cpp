#include "Manager.hpp"

namespace Engine
{
    namespace Mesh
    {
        namespace Manager
        {
            void Instance::addMesh(const std::string& inMeshId, const std::vector<Vertex::Base*>& inVertices)
            {
                if (meshesMap.find(inMeshId) != meshesMap.end())
                {
                    throw std::runtime_error("A Mesh identified by [" + inMeshId  + "] already exists");
                }

                Mesh::Instance newMesh;
                newMesh.vertices = inVertices;

                meshesOrder.push_back(inMeshId);
                meshesMap.insert(std::make_pair(inMeshId, newMesh));
            }

            Mesh::Instance Instance::getMesh(const std::string& inMeshId)
            {
                auto foundMesh = meshesMap.find(inMeshId);

                if (foundMesh == meshesMap.end())
                {
                    throw std::runtime_error("Invalid Mesh ID [" + inMeshId + "]");
                }

                return foundMesh->second;
            }

            void Instance::builMeshes(
                Vertex::Buffer::Instance& outVertexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                std::vector<Vertex::Base> extractedVertices;
                vk::DeviceSize extractedAllocationSize = 0;

                extractFromMeshList(extractedVertices, extractedAllocationSize);

                Vertex::Buffer::CreateInfo stagingBufferCreateInfo;
                stagingBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                stagingBufferCreateInfo.logicalDevice    = inLogicalDevice;
                stagingBufferCreateInfo.size             = extractedAllocationSize;
                stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
                stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

                Vertex::Buffer::Instance stagingBuffer;
                Vertex::Buffer::init(stagingBuffer, stagingBufferCreateInfo);

                memcpy(
                    inLogicalDevice.mapMemory(
                        stagingBuffer.memory,
                        0,
                        stagingBufferCreateInfo.size
                    ),
                    extractedVertices.data(),
                    stagingBufferCreateInfo.size
                );
                inLogicalDevice.unmapMemory(stagingBuffer.memory);

                Vertex::Buffer::CreateInfo bufferCreateInfo;
                bufferCreateInfo.physicalDevice   = inPhysicalDevice;
                bufferCreateInfo.logicalDevice    = inLogicalDevice;
                bufferCreateInfo.size             = extractedAllocationSize;
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

                Vertex::Buffer::init(outVertexBuffer, bufferCreateInfo);

                Vertex::Buffer::copy(
                    stagingBuffer,
                    outVertexBuffer,
                    extractedAllocationSize,
                    inQueue,
                    inCommandBuffer
                );

                Vertex::Buffer::destroy(inLogicalDevice, stagingBuffer);
            }

            void Instance::extractFromMeshList(std::vector<Vertex::Base>& outVertices, vk::DeviceSize& outAllocationSize)
            {
                uint32_t currentInstance = 0;

                for (std::string& meshId : meshesOrder)
                {
                    auto& meshInstance = meshesMap.find(meshId)->second;
    
                    AllocationInfo allocationInfo;
                    allocationInfo.vertexCount   = meshInstance.vertices.size();
                    allocationInfo.firstVertex   = outVertices.size();
                    allocationInfo.instanceCount = 1;
                    allocationInfo.firstInstance = currentInstance;

                    meshInstance.allocationInfo = allocationInfo;

                    outAllocationSize += sizeof(*meshInstance.vertices[0]) * allocationInfo.vertexCount;

                    for (auto vertex : meshInstance.vertices)
                    {
                        outVertices.push_back(*vertex);
                    }

                    currentInstance++;
                }
            }
        }
    }
}