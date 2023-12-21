#include "Manager.hpp"

namespace Chicane
{
    namespace Mesh
    {
        namespace Manager
        {
            void Instance::addMesh(
                const std::string& inMeshId,
                const std::vector<Vertex::Instance>& inVertices
            )
            {
                if (meshesMap.find(inMeshId) != meshesMap.end())
                {
                    throw std::runtime_error("The Mesh [" + inMeshId  + "] has already been initialized");
                }

                Mesh::Instance newMesh;
                newMesh.vertices = inVertices;

                meshesOrder.push_back(inMeshId);
                meshesMap.insert(std::make_pair(inMeshId, newMesh));
            }

            void Instance::drawMesh(const std::string& inMeshId, const vk::CommandBuffer& inCommadBuffer)
            {
                auto foundMesh = meshesMap.find(inMeshId);

                if (foundMesh == meshesMap.end())
                {
                    throw std::runtime_error("The Mesh [" + inMeshId + "] does not exist");
                }

                auto mesh = foundMesh->second;

                inCommadBuffer.draw(
                    mesh.allocationInfo.vertexCount,
                    mesh.allocationInfo.instanceCount,
                    mesh.allocationInfo.firstVertex,
                    mesh.allocationInfo.firstInstance
                );
            }

            void Instance::initVertexBuffer(
                Buffer::Instance& outVertexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                std::vector<Vertex::Instance> extractedVertices;
                vk::DeviceSize extractedAllocationSize = 0;

                extractFromMeshList(
                    extractedVertices,
                    extractedAllocationSize
                );

                Buffer::CreateInfo stagingBufferCreateInfo;
                stagingBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                stagingBufferCreateInfo.logicalDevice    = inLogicalDevice;
                stagingBufferCreateInfo.size             = extractedAllocationSize;
                stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
                stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                                           vk::MemoryPropertyFlagBits::eHostCoherent;

                Buffer::Instance stagingBuffer;
                Buffer::init(stagingBuffer, stagingBufferCreateInfo);

                void* writeLocation = inLogicalDevice.mapMemory(
                    stagingBuffer.memory,
                    0,
                    stagingBufferCreateInfo.size
                );
                memcpy(
                    writeLocation,
                    extractedVertices.data(),
                    stagingBufferCreateInfo.size
                );
                inLogicalDevice.unmapMemory(stagingBuffer.memory);

                Buffer::CreateInfo bufferCreateInfo;
                bufferCreateInfo.physicalDevice   = inPhysicalDevice;
                bufferCreateInfo.logicalDevice    = inLogicalDevice;
                bufferCreateInfo.size             = extractedAllocationSize;
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                                    vk::BufferUsageFlagBits::eVertexBuffer;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

                Buffer::init(outVertexBuffer, bufferCreateInfo);

                Buffer::copy(
                    stagingBuffer,
                    outVertexBuffer,
                    extractedAllocationSize,
                    inQueue,
                    inCommandBuffer
                );

                Buffer::destroy(inLogicalDevice, stagingBuffer);
            }

            void Instance::extractFromMeshList(
                std::vector<Vertex::Instance>& outVertices,
                vk::DeviceSize& outAllocationSize
            )
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

                    outAllocationSize += sizeof(meshInstance.vertices[0]) * allocationInfo.vertexCount;

                    for (auto vertex : meshInstance.vertices)
                    {
                        outVertices.push_back(vertex);
                    }

                    currentInstance++;
                }
            }
        }
    }
}