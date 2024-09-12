#include "Chicane/Renderer/Model/Manager.hpp"

namespace Chicane
{
    namespace Model
    {
        Manager::Manager()
            : m_observable(std::make_unique<Observable<EventSubject>>())
        {}

        bool Manager::isEmpty() const
        {
            return m_vertices.empty();
        }

        bool Manager::canDraw() const
        {
            return !isEmpty() && !m_usedIds.empty();
        }

        bool Manager::isLoaded(const std::string& inId) const
        {
            return m_instanceMap.find(inId) != m_instanceMap.end();
        }

        bool Manager::isAllocated(const std::string& inId) const
        {
            return m_allocationMap.find(inId) != m_allocationMap.end();
        }

        bool Manager::isUsing(const std::string& inId) const
        {
            return std::find(
                m_usedIds.begin(),
                m_usedIds.end(),
                inId
            ) != m_usedIds.end();
        }

        const Vec<3, float>& Manager::getBounds(const std::string& inId)
        {
            if (!isLoaded(inId))
            {
                return Vec3Zero;
            }

            return m_instanceMap.at(inId).extent;
        }

        void Manager::load(const std::string& inId, const Box::Entry& inEntry)
        {
            if (isLoaded(inId))
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

            Model::Instance newModel {};
            newModel.vertexInstances = result.vertices;
            newModel.vertexIndices   = result.indexes;
            newModel.extent          = getExtent(result.vertices);

            m_instanceMap.insert(std::make_pair(inId, newModel));

            m_observable->next(EventSubject::Load);
        }

        void Manager::activate(const std::string& inId)
        {
            if (!isLoaded(inId))
            {
                return;
            }

            m_usedIds.push_back(inId);

            if (isAllocated(inId))
            {
                m_observable->next(EventSubject::Use);

                return;
            }

            allocate(inId);

            m_observable->next(EventSubject::Allocation);
        }

        void Manager::deactivate(const std::string& inId)
        {
            if (!isLoaded(inId) || !isAllocated(inId) || !isUsing(inId))
            {
                return;
            }

            m_usedIds.erase(
                std::find(
                    m_usedIds.begin(),
                    m_usedIds.end(),
                    inId
                )
            );

            if (isUsing(inId))
            {
                m_observable->next(EventSubject::Use);

                return;
            }

            deallocate(inId);

            m_observable->next(EventSubject::Allocation);
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
            if (isEmpty())
            {
                return;
            }

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

        std::uint32_t Manager::getUseCount(const std::string& inId) const
        {
            if (!isUsing(inId))
            {
                return 0;
            }

            return std::count(
                m_usedIds.begin(),
                m_usedIds.end(),
                inId
            );
        }

        std::uint32_t Manager::getFirstInstance(const std::string& inId) const
        {
            if (!isLoaded(inId))
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            std::uint32_t result = static_cast<std::uint32_t>(
                std::find(
                    m_usedIds.begin(),
                    m_usedIds.end(),
                    inId
                ) - m_usedIds.begin()
            );

            return result;
        }

        void Manager::draw(
            const std::string& inId,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            if (!canDraw() || !isUsing(inId))
            {
                return;
            }

            if (!isLoaded(inId) || !isAllocated(inId))
            {
                throw std::runtime_error("The Model [" + inId + "] does not exist");
            }

            const Model::AllocationInfo& allocationInfo = m_allocationMap.at(inId);

            inCommandBuffer.drawIndexed(
                allocationInfo.indexCount,
                getUseCount(inId),
                allocationInfo.firstIndex,
                0,
                getFirstInstance(inId)
            );
        }

        void Manager::drawAll(const vk::CommandBuffer& inCommandBuffer)
        {
            for (auto& [id, data] : m_allocationMap)
            {
                draw(id, inCommandBuffer);
            }
        }

        void Manager::watchChanges(
            std::function<void (EventSubject)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback,
            std::function<void ()> inCompleteCallback
        )
        {
            m_observable->subscribe(
                inNextCallback,
                inErrorCallback,
                inCompleteCallback
            );
        }

        void Manager::allocate(const std::string& inId)
        {
            if (!isLoaded(inId) || isAllocated(inId))
            {
                return;
            }

            const Model::Instance& instance = m_instanceMap.at(inId);

            AllocationInfo allocationInfo {};
            allocationInfo.vertexCount = static_cast<uint32_t>(instance.vertexInstances.size());
            allocationInfo.firstVertex = static_cast<uint32_t>(m_vertices.size());
            allocationInfo.indexCount  = static_cast<uint32_t>(instance.vertexIndices.size());
            allocationInfo.firstIndex  = static_cast<uint32_t>(m_indices.size());

            m_allocationMap.insert(std::make_pair(inId, allocationInfo));

            m_vertices.insert(
                m_vertices.end(),
                instance.vertexInstances.begin(),
                instance.vertexInstances.end()
            );

            for (uint32_t vertexIndex : instance.vertexIndices)
            {
                m_indices.push_back(vertexIndex + allocationInfo.firstVertex);
            }
        }

        void Manager::deallocate(const std::string& inId)
        {
            if (!isLoaded(inId) || !isAllocated(inId))
            {
                return;
            }

            const AllocationInfo& instance = m_allocationMap.at(inId);

            auto verticesStart = m_vertices.begin() + instance.firstVertex;
            auto verticesEnd   = verticesStart + instance.vertexCount;
            m_vertices.erase(verticesStart, verticesEnd);

            auto indicesStart = m_indices.begin() + instance.firstIndex;
            auto indicesEnd   = indicesStart + instance.indexCount;
            m_indices.erase(indicesStart, indicesEnd);

            m_allocationMap.erase(inId);

            rebaseAllocation();
        }

        void Manager::rebaseAllocation()
        {
            std::uint32_t firstVertex = 0;
            std::uint32_t firstIndex  = 0;

            for (const std::string& id : m_usedIds)
            {
                AllocationInfo& instance = m_allocationMap.at(id);
                instance.firstVertex = firstVertex;
                instance.firstIndex  = firstIndex;

                firstVertex += instance.vertexCount;
                firstIndex  += instance.indexCount;
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
            stagingBufferCreateInfo.size             = sizeof(Vertex::Instance) * m_vertices.size();
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
                m_vertices.data(),
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
            stagingBufferCreateInfo.size             = sizeof(uint32_t) * m_indices.size();
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
                m_indices.data(),
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

        Vec<3, float> Manager::getExtent(const std::vector<Vertex::Instance>& inVertices)
        {
            float xMin = FLT_MAX;
            float xMax = -FLT_MAX;

            float yMin = FLT_MAX;
            float yMax = -FLT_MAX;

            float zMin = FLT_MAX;
            float zMax = -FLT_MAX;

            for (const Vertex::Instance& vertex : inVertices)
            {
                const Vec<3, float>& position = vertex.position;

                if (position.x < xMin)
                {
                    xMin = position.x;
                }

                if (position.x > xMax)
                {
                    xMax = position.x;
                }

                if (position.y < yMin)
                {
                    yMin = position.y;
                }

                if (position.y > yMax)
                {
                    yMax = position.y;
                }

                if (position.z < zMin)
                {
                    zMin = position.z;
                }

                if (position.z > zMax)
                {
                    zMax = position.z;
                }
            }

            Vec<3, float> result {};
            result.x = xMax - xMin;
            result.y = yMax - yMin;
            result.z = zMax - zMin;

            return result;
        }
    }
}