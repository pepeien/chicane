#include "Manager.hpp"

namespace Chicane
{
    namespace Model
    {
        namespace Manager
        {
            std::vector<std::string> Instance::getModelIds()
            {
                return m_registeredModelIds;
            }

            void Instance::add(
                const std::string& inId,
                const std::string& inFilepath,
                Vendor inVendor = Vendor::Undefined
            )
            {
                if (m_ModelInstances.find(inId) != m_ModelInstances.end())
                {
                    throw std::runtime_error("The Model [" + inId  + "] has already been added");
                }

                ParseResult result;

                switch (inVendor)
                {
                case Vendor::Wavefront:
                    result = Wavefront::parse(inFilepath);

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

            void Instance::buildAll(
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

            void Instance::draw(
                const std::string& inId,
                const vk::CommandBuffer& inCommadBuffer,
                uint32_t inInstanceCount,
                uint32_t inFirstInstance
            )
            {
                auto foundModel = m_ModelAllocationInfos.find(inId);

                if (foundModel == m_ModelAllocationInfos.end())
                {
                    throw std::runtime_error("The Model [" + inId + "] does not exist");
                }

                Model::AllocationInfo& allocationInfo = foundModel->second;

                inCommadBuffer.drawIndexed(
                    allocationInfo.indexCount,
                    inInstanceCount,
                    allocationInfo.firstIndex,
                    0,
                    inFirstInstance
                );
            }

            void Instance::addModel(
                const std::string& inId,
                const std::vector<Vertex::Instance>& inVertices,
                const std::vector<uint32_t>& inIndexes
            )
            {
                if (m_ModelInstances.find(inId) != m_ModelInstances.end())
                {
                    throw std::runtime_error("The Model [" + inId  + "] has already been added");
                }

                Model::Instance newModel;
                newModel.vertexInstances = inVertices;
                newModel.vertexIndices   = inIndexes;

                m_registeredModelIds.push_back(inId);
                m_ModelInstances.insert(std::make_pair(inId, newModel));
            }

            void Instance::processModels()
            {
                for (std::string& ModelId : m_registeredModelIds)
                {
                    auto foundModel = m_ModelInstances.find(ModelId);

                    if (foundModel == m_ModelInstances.end())
                    {
                        throw std::runtime_error("The Model [" + ModelId + "] does not exist");
                    }

                    Model::Instance& ModelInstance = foundModel->second;
    
                    AllocationInfo allocationInfo;
                    allocationInfo.vertexCount   = ModelInstance.vertexInstances.size();
                    allocationInfo.firstVertex   = m_combinedVertices.size();
                    allocationInfo.indexCount    = ModelInstance.vertexIndices.size();
                    allocationInfo.firstIndex    = m_indexedVertices.size();

                    m_ModelAllocationInfos.insert(std::make_pair(ModelId, allocationInfo));

                    m_combinedVertices.insert(
                        m_combinedVertices.end(),
                        ModelInstance.vertexInstances.begin(),
                        ModelInstance.vertexInstances.end()
                    );

                    for (uint32_t vertexIndex : ModelInstance.vertexIndices)
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