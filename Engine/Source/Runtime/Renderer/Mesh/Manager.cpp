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
                if (meshInstances.find(inMeshId) != meshInstances.end())
                {
                    throw std::runtime_error("The Mesh [" + inMeshId  + "] has already been initialized");
                }

                Mesh::Instance newMesh;
                newMesh.verticeInstances = inVertices;

                registeredMeshIds.push_back(inMeshId);
                meshInstances.insert(std::make_pair(inMeshId, newMesh));
            }

            void Instance::drawMesh(const std::string& inMeshId, const vk::CommandBuffer& inCommadBuffer)
            {
                auto foundMesh = meshAllocationInfos.find(inMeshId);

                if (foundMesh == meshAllocationInfos.end())
                {
                    throw std::runtime_error("The Mesh [" + inMeshId + "] does not exist");
                }

                Mesh::AllocationInfo allocationInfo = foundMesh->second;

                inCommadBuffer.drawIndexed(
                    allocationInfo.vertexCount,
                    allocationInfo.instanceCount,
                    allocationInfo.firstVertex,
                    0,
                    allocationInfo.firstInstance
                );
            }

            void Instance::initBuffers(
                Buffer::Instance& outVertexBuffer,
                Buffer::Instance& outIndexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                setup();

                initVertexBuffer(
                    outVertexBuffer,
                    inLogicalDevice,
                    inPhysicalDevice,
                    inQueue,
                    inCommandBuffer
                );

                initIndexBuffer(
                    outIndexBuffer,
                    inLogicalDevice,
                    inPhysicalDevice,
                    inQueue,
                    inCommandBuffer
                );
            }

            void Instance::setup()
            {
                for (uint32_t i = 0; i < registeredMeshIds.size(); i++)
                {
                    std::string& meshId          = registeredMeshIds[i];
                    Mesh::Instance& meshInstance = meshInstances.find(meshId)->second;
    
                    AllocationInfo allocationInfo;
                    allocationInfo.vertexCount   = meshInstance.verticeInstances.size();
                    allocationInfo.firstVertex   = combinedVertices.size();
                    allocationInfo.instanceCount = 1;
                    allocationInfo.firstInstance = i;

                    meshAllocationInfos[meshId] = allocationInfo;

                    for (Vertex::Instance vertex : meshInstance.verticeInstances)
                    {
                        combinedVertices.push_back(vertex);
                    }
                }

                indexedVertices.resize(combinedVertices.size());

                for (int i = 0; i < combinedVertices.size(); i++)
                {
                    Vertex::Instance vertex = combinedVertices[i];

                    for (int j = 0; j < combinedVertices.size(); j++)
                    {
                        Vertex::Instance subVertex = combinedVertices[j];

                        if (
                            fabs(vertex.position.x - subVertex.position.x) > FLT_EPSILON ||
                            fabs(vertex.position.y - subVertex.position.y) > FLT_EPSILON
                        )
                        {
                            continue;
                        }

                        indexedVertices[i] = j;

                        break;
                    }
                }
            }
            
            void Instance::initVertexBuffer(
                Buffer::Instance& outVertexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                Buffer::CreateInfo stagingBufferCreateInfo;
                stagingBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                stagingBufferCreateInfo.logicalDevice    = inLogicalDevice;
                stagingBufferCreateInfo.size             = sizeof(Vertex::Instance) * combinedVertices.size();
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
                    combinedVertices.data(),
                    stagingBufferCreateInfo.size
                );
                inLogicalDevice.unmapMemory(stagingBuffer.memory);

                Buffer::CreateInfo bufferCreateInfo;
                bufferCreateInfo.physicalDevice   = inPhysicalDevice;
                bufferCreateInfo.logicalDevice    = inLogicalDevice;
                bufferCreateInfo.size             = stagingBufferCreateInfo.size;
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                                    vk::BufferUsageFlagBits::eVertexBuffer;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

                Buffer::init(outVertexBuffer, bufferCreateInfo);

                Buffer::copy(
                    stagingBuffer,
                    outVertexBuffer,
                    bufferCreateInfo.size,
                    inQueue,
                    inCommandBuffer
                );

                Buffer::destroy(inLogicalDevice, stagingBuffer);
            }

            void Instance::initIndexBuffer(
                Buffer::Instance& outIndexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                Buffer::CreateInfo stagingBufferCreateInfo;
                stagingBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                stagingBufferCreateInfo.logicalDevice    = inLogicalDevice;
                stagingBufferCreateInfo.size             = sizeof(uint32_t) * indexedVertices.size();
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
                    indexedVertices.data(),
                    stagingBufferCreateInfo.size
                );
                inLogicalDevice.unmapMemory(stagingBuffer.memory);

                Buffer::CreateInfo bufferCreateInfo;
                bufferCreateInfo.physicalDevice   = inPhysicalDevice;
                bufferCreateInfo.logicalDevice    = inLogicalDevice;
                bufferCreateInfo.size             = stagingBufferCreateInfo.size;
                bufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferDst |
                                                    vk::BufferUsageFlagBits::eIndexBuffer;
                bufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

                Buffer::init(outIndexBuffer, bufferCreateInfo);

                Buffer::copy(
                    stagingBuffer,
                    outIndexBuffer,
                    bufferCreateInfo.size,
                    inQueue,
                    inCommandBuffer
                );

                Buffer::destroy(inLogicalDevice, stagingBuffer);
            }
        }
    }
}