#include "Backend/Vulkan/Buffer.hpp"

#include <stdexcept>

#include "Backend/Vulkan/Allocator.hpp"
#include "Backend/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanBuffer::VulkanBuffer()
            : instance(nullptr),
              memory(nullptr),
              mapped(nullptr),
              owner(nullptr),
              vma(nullptr),
              allocation(nullptr),
              size(0),
              usage(),
              properties()
        {}

        void VulkanBuffer::init(const VulkanBufferCreateInfo& inCreateInfo)
        {
            if (!inCreateInfo.allocator)
            {
                throw std::runtime_error("Vulkan buffer requires an allocator");
            }

            inCreateInfo.allocator->createBuffer(*this, inCreateInfo);
        }

        void VulkanBuffer::destroy()
        {
            if (!owner)
            {
                *this = {};

                return;
            }

            owner->destroyBuffer(*this);
        }

        void* VulkanBuffer::map()
        {
            if (!owner)
            {
                return mapped;
            }

            return owner->map(*this);
        }

        void VulkanBuffer::unmap()
        {
            if (!owner)
            {
                return;
            }

            owner->unmap(*this);
        }

        void VulkanBuffer::copy(
            const VulkanBuffer&      inDestination,
            const vk::DeviceSize&    inAllocationSize,
            const vk::Queue&         inQueue,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            VulkanCommandBufferWorker::startJob(inCommandBuffer);

            vk::BufferCopy copyRegion;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size      = inAllocationSize;

            inCommandBuffer.copyBuffer(instance, inDestination.instance, 1, &copyRegion);

            VulkanCommandBufferWorker::endJob(inCommandBuffer, inQueue, "Copy The Buffer");
        }
    }
}
