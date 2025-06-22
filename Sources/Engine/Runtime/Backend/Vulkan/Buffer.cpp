#include "Chicane/Runtime/Backend/Vulkan/Buffer.hpp"

#include "Chicane/Runtime/Backend/Vulkan/Device.hpp"
#include "Chicane/Runtime/Backend/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        Buffer::Buffer()
            : instance(nullptr),
            memory(nullptr)
        {}

        void Buffer::init(const BufferCreateInfo& inCreateInfo)
        {
            vk::BufferCreateInfo bufferInfo = {};
            bufferInfo.size        = inCreateInfo.size;
            bufferInfo.usage       = inCreateInfo.usage;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            instance = inCreateInfo.logicalDevice.createBuffer(bufferInfo);

            allocate(inCreateInfo);
        }

        void Buffer::allocate(const BufferCreateInfo& inCreateInfo)
        {
            vk::MemoryRequirements memoryRequirements = inCreateInfo
                .logicalDevice
                .getBufferMemoryRequirements(instance);

            vk::MemoryAllocateInfo memoryAlocateInfo = {};
            memoryAlocateInfo.allocationSize  = memoryRequirements.size;
            memoryAlocateInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                inCreateInfo.physicalDevice,
                memoryRequirements.memoryTypeBits,
                inCreateInfo.memoryProperties
            );

            memory = inCreateInfo.logicalDevice.allocateMemory(memoryAlocateInfo);

            inCreateInfo.logicalDevice.bindBufferMemory(instance, memory, 0);
        }

        void Buffer::copy(
            const Buffer& inDestination,
            const vk::DeviceSize& inAllocationSize,
            const vk::Queue& inQueue,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            CommandBuffer::Worker::startJob(inCommandBuffer);

            vk::BufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size      = inAllocationSize;

            inCommandBuffer.copyBuffer(
                instance,
                inDestination.instance,
                1,
                &copyRegion
            );

            CommandBuffer::Worker::endJob(inCommandBuffer, inQueue, "Copy The Buffer");
        }

        void Buffer::destroy(const vk::Device& inLogicalDevice)
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