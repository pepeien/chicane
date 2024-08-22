#include "Chicane/Renderer/Model/Manager.hpp"

namespace Chicane
{
    namespace Model
    {
        bool Manager::contains(const std::string& inId) const
        {
            return m_instanceMap.find(inId) != m_instanceMap.end();
        }

        void Manager::use(const std::string& inId)
        {
            if (!contains(inId))
            {
                return;
            }

            m_allocationMap.at(inId).instanceCount += 1;
            m_usedIds.push_back(inId);
        }

        void Manager::add(const std::string& inId, const Box::Entry& inEntry)
        {
            if (contains(inId))
            {
                return;
            }

            ParseResult result {};

            switch (static_cast<Vendor>(inEntry.vendor))
            {
            case Vendor::Wavefront:
                Wavefront::parse(result, inEntry.data);

                break;

            default:
                throw std::runtime_error("Failed to import Model due to invalid type");
            }

            Model::Instance newModel;
            newModel.vertexInstances = result.vertices;
            newModel.vertexIndices   = result.indexes;

            m_uniqueIds.push_back(inId);
            m_instanceMap.insert(std::make_pair(inId, newModel));

            process(inId);
            use(inId);
        }

        void Manager::build(
            Buffer::Instance& outVertexBuffer,
            Buffer::Instance& outIndexBuffer,
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::Queue& inQueue,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
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

        std::uint32_t Manager::getFirstInstance(const std::string& inId) const
        {
            if (!contains(inId))
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            std::uint32_t result = static_cast<std::uint32_t>(
                std::find(
                    m_uniqueIds.begin(),
                    m_uniqueIds.end(),
                    inId
                ) - m_uniqueIds.begin()
            );

            return result;
        }

        void Manager::draw(
            const std::string& inId,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            auto foundModel = m_allocationMap.find(inId);

            if (foundModel == m_allocationMap.end())
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            Model::AllocationInfo& allocationInfo = foundModel->second;

            if (allocationInfo.instanceCount == 0)
            {
                return;
            }

            inCommandBuffer.drawIndexed(
                allocationInfo.indexCount,
                allocationInfo.instanceCount,
                allocationInfo.firstIndex,
                0,
                getFirstInstance(inId)
            );
        }

        void Manager::drawAll(const vk::CommandBuffer& inCommandBuffer)
        {
            for (std::string& id : m_uniqueIds)
            {
                draw(id, inCommandBuffer);
            }
        }

        void Manager::process(const std::string& inId)
        {
            auto foundModel = m_instanceMap.find(inId);

            if (foundModel == m_instanceMap.end())
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            if (m_allocationMap.find(inId) != m_allocationMap.end())
            {
                return;
            }

            Model::Instance& instance = foundModel->second;

            AllocationInfo allocationInfo;
            allocationInfo.vertexCount   = static_cast<uint32_t>(instance.vertexInstances.size());
            allocationInfo.firstVertex   = static_cast<uint32_t>(m_combinedVertices.size());
            allocationInfo.indexCount    = static_cast<uint32_t>(instance.vertexIndices.size());
            allocationInfo.instanceCount = 0;
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
        
        void Manager::initVertexBuffer(
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

        void Manager::initIndexBuffer(
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