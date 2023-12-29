#include "Manager.hpp"

namespace Chicane
{
    namespace Mesh
    {
        namespace Manager
        {
            std::vector<std::string> Instance::getMeshIds()
            {
                return m_registeredMeshIds;
            }

            void Instance::addMesh(
                const std::string& inId,
                const std::vector<Vertex::Instance>& inVertices,
                const std::vector<uint32_t>& inIndexes
            )
            {
                if (m_meshInstances.find(inId) != m_meshInstances.end())
                {
                    throw std::runtime_error("The Mesh [" + inId  + "] has already been added");
                }

                Mesh::Instance newMesh;
                newMesh.vertexInstances = inVertices;
                newMesh.vertexIndices   = inIndexes;

                m_registeredMeshIds.push_back(inId);
                m_meshInstances.insert(std::make_pair(inId, newMesh));
            }

            void Instance::importMesh(
                const std::string& inId,
                const std::string& inFilepath,
                Type inType = Type::Undefined
            )
            {
                if (m_meshInstances.find(inId) != m_meshInstances.end())
                {
                    throw std::runtime_error("The Mesh [" + inId  + "] has already been added");
                }

                ParseResult result;

                switch (inType)
                {
                case Type::Wavefront:
                    result = Wavefront::parse(inFilepath);

                    break;
                
                default:
                    throw std::runtime_error("Failed to import mesh due to invalid type");
                }

                addMesh(
                    inId,
                    result.vertices,
                    result.indexes
                );
            }

            void Instance::drawMesh(
                const std::string& inId,
                const vk::CommandBuffer& inCommadBuffer,
                uint32_t inInstanceCount,
                uint32_t inFirstInstance
            )
            {
                auto foundMesh = m_meshAllocationInfos.find(inId);

                if (foundMesh == m_meshAllocationInfos.end())
                {
                    throw std::runtime_error("The Mesh [" + inId + "] does not exist");
                }

                Mesh::AllocationInfo& allocationInfo = foundMesh->second;

                inCommadBuffer.drawIndexed(
                    allocationInfo.indexCount,
                    inInstanceCount,
                    allocationInfo.firstIndex,
                    0,
                    inFirstInstance
                );
            }

            void Instance::loadMeshes(
                Buffer::Instance& outVertexBuffer,
                Buffer::Instance& outIndexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                processMeshes();

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

            void Instance::processMeshes()
            {
                for (std::string& meshId : m_registeredMeshIds)
                {
                    auto foundMesh = m_meshInstances.find(meshId);

                    if (foundMesh == m_meshInstances.end())
                    {
                        throw std::runtime_error("The Mesh [" + meshId + "] does not exist");
                    }

                    Mesh::Instance& meshInstance = foundMesh->second;
    
                    AllocationInfo allocationInfo;
                    allocationInfo.vertexCount   = meshInstance.vertexInstances.size();
                    allocationInfo.firstVertex   = m_combinedVertices.size();
                    allocationInfo.indexCount    = meshInstance.vertexIndices.size();
                    allocationInfo.firstIndex    = m_indexedVertices.size();

                    m_meshAllocationInfos.insert(std::make_pair(meshId, allocationInfo));

                    m_combinedVertices.insert(
                        m_combinedVertices.end(),
                        meshInstance.vertexInstances.begin(),
                        meshInstance.vertexInstances.end()
                    );

                    for (uint32_t vertexIndex : meshInstance.vertexIndices)
                    {
                        m_indexedVertices.push_back(vertexIndex + allocationInfo.firstVertex);
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
                stagingBufferCreateInfo.size             = sizeof(Vertex::Instance) * m_combinedVertices.size();
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
                    m_combinedVertices.data(),
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
                stagingBufferCreateInfo.size             = sizeof(uint32_t) * m_indexedVertices.size();
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
                    m_indexedVertices.data(),
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