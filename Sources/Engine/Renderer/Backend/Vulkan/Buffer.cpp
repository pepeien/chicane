#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"

#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Worker.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Device.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanBuffer::VulkanBuffer()
            : instance(nullptr),
              memory(nullptr)
        {}

        void VulkanBuffer::init(const VulkanBufferCreateInfo& inCreateInfo)
        {
            vk::BufferCreateInfo bufferInfo = {};
            bufferInfo.size                 = inCreateInfo.size;
            bufferInfo.usage                = inCreateInfo.usage;
            bufferInfo.sharingMode          = vk::SharingMode::eExclusive;

            instance = inCreateInfo.logicalDevice.createBuffer(bufferInfo);

            allocate(inCreateInfo);
        }

        void VulkanBuffer::allocate(const VulkanBufferCreateInfo& inCreateInfo)
        {
            vk::MemoryRequirements memoryRequirements =
                inCreateInfo.logicalDevice.getBufferMemoryRequirements(instance);

            vk::MemoryAllocateInfo memoryAlocateInfo = {};
            memoryAlocateInfo.allocationSize         = memoryRequirements.size;
            memoryAlocateInfo.memoryTypeIndex        = VulkanDevice::findMemoryTypeIndex(
                inCreateInfo.physicalDevice,
                memoryRequirements.memoryTypeBits,
                inCreateInfo.memoryProperties
            );

            memory = inCreateInfo.logicalDevice.allocateMemory(memoryAlocateInfo);

            inCreateInfo.logicalDevice.bindBufferMemory(instance, memory, 0);
        }

        void VulkanBuffer::copy(
            const VulkanBuffer&      inDestination,
            const vk::DeviceSize&    inAllocationSize,
            const vk::Queue&         inQueue,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            VulkanCommandBufferWorker::startJob(inCommandBuffer);

            vk::BufferCopy copyRegion = {};
            copyRegion.srcOffset      = 0;
            copyRegion.dstOffset      = 0;
            copyRegion.size           = inAllocationSize;

            inCommandBuffer.copyBuffer(instance, inDestination.instance, 1, &copyRegion);

            VulkanCommandBufferWorker::endJob(inCommandBuffer, inQueue, "Copy The Buffer");
        }

        void VulkanBuffer::destroy(const vk::Device& inLogicalDevice)
        {
            if (instance)
            {
                inLogicalDevice.destroyBuffer(instance);
                instance = nullptr;
            }

            if (memory)
            {
                inLogicalDevice.freeMemory(memory);
                memory = nullptr;
            }
        }
    }
}