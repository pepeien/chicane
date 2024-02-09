#include "Runtime/Renderer/Model/Manager.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Manager
        {
            void Instance::add(
                const std::string& inId,
                const std::vector<unsigned char>& inData,
                Vendor inVendor
            )
            {
                if (m_instanceMap.find(inId) != m_instanceMap.end())
                {
                    addDuplicate(inId);

                    return;
                }

                ParseResult result;

                switch (inVendor)
                {
                case Vendor::Wavefront:
                    result = Wavefront::parse(inData);

                    break;
                
                default:
                    throw std::runtime_error("Failed to import Model due to invalid type");
                }

                addModel(
                    inId,
                    result.vertices,
                    result.indexes
                );
            }

            void Instance::build(
                Buffer::Instance& outVertexBuffer,
                Buffer::Instance& outIndexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            )
            {
                processModels();

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

            void Instance::drawAll(const vk::CommandBuffer& inCommandBuffer)
            {
                for (uint32_t i = 0; i < m_uniqueIds.size(); i++)
                {
                    draw(
                        m_uniqueIds[i],
                        inCommandBuffer,
                        i
                    );
                }
            }

            void Instance::addDuplicate(const std::string& inId)
            {
                m_usedIds.push_back(inId);
            }

            void Instance::addModel(
                const std::string& inId,
                const std::vector<Vertex::Instance>& inVertices,
                const std::vector<uint32_t>& inIndexes
            )
            {
                if (m_instanceMap.find(inId) != m_instanceMap.end())
                {
                    LOG_WARNING("The model [" + inId  + "] has already been added");

                    return;
                }

                Model::Instance newModel;
                newModel.vertexInstances = inVertices;
                newModel.vertexIndices   = inIndexes;

                m_uniqueIds.push_back(inId);
                m_usedIds.push_back(inId);
                m_instanceMap.insert(std::make_pair(inId, newModel));
            }

            void Instance::processDuplicate(const std::string& inId)
            {
                m_allocationMap[inId].instanceCount += 1;
            }

            void Instance::processModel(const std::string& inId)
            {
                auto foundModel = m_instanceMap.find(inId);

                if (foundModel == m_instanceMap.end())
                {
                    throw std::runtime_error("The Model [" + inId + "] does not exist");
                }

                Model::Instance& instance = foundModel->second;
    
                AllocationInfo allocationInfo;
                allocationInfo.vertexCount   = static_cast<uint32_t>(instance.vertexInstances.size());
                allocationInfo.firstVertex   = static_cast<uint32_t>(m_combinedVertices.size());
                allocationInfo.indexCount    = static_cast<uint32_t>(instance.vertexIndices.size());
                allocationInfo.instanceCount = 1;
                allocationInfo.firstIndex    = static_cast<uint32_t>(m_indexedVertices.size());

                m_allocationMap.insert(std::make_pair(inId, allocationInfo));

                m_combinedVertices.insert(
                    m_combinedVertices.end(),
                    instance.vertexInstances.begin(),
                    instance.vertexInstances.end()
                );

                for (uint32_t vertexIndex : instance.vertexIndices)
                {
                    m_indexedVertices.push_back(vertexIndex + allocationInfo.firstVertex);
                }
            }

            void Instance::processModels()
            {
                for (std::string& id : m_usedIds)
                {
                    if (m_allocationMap.find(id) != m_allocationMap.end())
                    {
                        processDuplicate(id);

                        continue;
                    }

                    processModel(id);
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

            void Instance::draw(
                const std::string& inId,
                const vk::CommandBuffer& inCommandBuffer,
                uint32_t inFirstInstance
            )
            {
                auto foundModel = m_allocationMap.find(inId);

                if (foundModel == m_allocationMap.end())
                {
                    throw std::runtime_error("The Model [" + inId + "] does not exist");
                }

                Model::AllocationInfo& allocationInfo = foundModel->second;

                inCommandBuffer.drawIndexed(
                    allocationInfo.indexCount,
                    allocationInfo.instanceCount,
                    allocationInfo.firstIndex,
                    0,
                    inFirstInstance
                );
            }
        }
    }
}