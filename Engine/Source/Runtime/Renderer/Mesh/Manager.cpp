#include "Manager.hpp"

namespace Chicane
{
    namespace Mesh
    {
        namespace Manager
        {
            void Instance::importMesh(
                const std::string& inMeshId,
                const std::string& inFilepath,
                Type inType = Type::Undefined
            )
            {
                ParseResult result;

                switch (inType)
                {
                case Type::Wavefront:
                    result = Wavefront::parse(inFilepath);

                    break;
                
                default:
                    throw std::runtime_error("Failed to add mesh due to invalid type");
                }

                addMesh(
                    inMeshId,
                    result.vertices,
                    result.indexes
                );
            }

            void Instance::drawMesh(
                const std::string& inId,
                uint32_t inInstanceCount,
                const vk::CommandBuffer& inCommadBuffer
            )
            {
                auto foundMesh = m_meshAllocationInfos.find(inId);

                if (foundMesh == m_meshAllocationInfos.end())
                {
                    throw std::runtime_error("The Mesh [" + inId + "] does not exist");
                }

                Mesh::AllocationInfo allocationInfo = foundMesh->second;

                inCommadBuffer.drawIndexed(
                    allocationInfo.vertexCount,
                    inInstanceCount,
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

            void Instance::addMesh(
                const std::string& inMeshId,
                const std::vector<Vertex::Instance>& inVertices,
                const std::vector<uint32_t>& inIndexes
            )
            {
                if (m_meshInstances.find(inMeshId) != m_meshInstances.end())
                {
                    throw std::runtime_error("The Mesh [" + inMeshId  + "] has already been initialized");
                }

                Mesh::Instance newMesh;
                newMesh.vertexInstances = inVertices;

                m_registeredMeshIds.push_back(inMeshId);
                m_meshInstances.insert(std::make_pair(inMeshId, newMesh));
                m_indexedVertices.insert(
                    m_indexedVertices.end(),
                    inIndexes.begin(),
                    inIndexes.end()
                );
            }

            void Instance::setup()
            {
                for (uint32_t i = 0; i < m_registeredMeshIds.size(); i++)
                {
                    std::string& meshId          = m_registeredMeshIds[i];
                    Mesh::Instance& meshInstance = m_meshInstances.find(meshId)->second;
    
                    AllocationInfo allocationInfo;
                    allocationInfo.vertexCount   = meshInstance.vertexInstances.size();
                    allocationInfo.firstVertex   = m_combinedVertices.size();
                    allocationInfo.firstInstance = i;

                    m_meshAllocationInfos[meshId] = allocationInfo;

                    for (Vertex::Instance vertex : meshInstance.vertexInstances)
                    {
                        m_combinedVertices.push_back(vertex);
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