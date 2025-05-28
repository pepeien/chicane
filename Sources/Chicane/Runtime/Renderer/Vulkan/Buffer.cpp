#include "Chicane/Runtime/Renderer/Vulkan/Buffer.hpp"

#include "Chicane/Runtime/Renderer/Vulkan/Device.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Buffer
        {
            void init(Instance& outBuffer, const CreateInfo& inCreateInfo)
            {
                vk::BufferCreateInfo bufferInfo = {};
                bufferInfo.size        = inCreateInfo.size;
                bufferInfo.usage       = inCreateInfo.usage;
                bufferInfo.sharingMode = vk::SharingMode::eExclusive;

                outBuffer.instance = inCreateInfo.logicalDevice.createBuffer(bufferInfo);

                allocate(outBuffer, inCreateInfo);
            }

            void allocate(Instance& outBuffer, const CreateInfo& inCreateInfo)
            {
                vk::MemoryRequirements memoryRequirements = inCreateInfo
                    .logicalDevice
                    .getBufferMemoryRequirements(outBuffer.instance);

                vk::MemoryAllocateInfo memoryAlocateInfo = {};
                memoryAlocateInfo.allocationSize  = memoryRequirements.size;
                memoryAlocateInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                    inCreateInfo.physicalDevice,
                    memoryRequirements.memoryTypeBits,
                    inCreateInfo.memoryProperties
                );

                outBuffer.memory = inCreateInfo.logicalDevice.allocateMemory(memoryAlocateInfo);

                inCreateInfo.logicalDevice.bindBufferMemory(
                    outBuffer.instance,
                    outBuffer.memory,
                    0
                );
            }

            void copy(
                const Instance& inSourceBuffer,
                const Instance& inDestinationBuffer,
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
                    inSourceBuffer.instance,
                    inDestinationBuffer.instance,
                    1,
                    &copyRegion
                );

                CommandBuffer::Worker::endJob(inCommandBuffer, inQueue, "Copy The Buffer");
            }

            void destroy(const vk::Device& inLogicalDevice, Instance& inBuffer)
            {
                if (inBuffer.instance)
                {
                    inLogicalDevice.destroyBuffer(inBuffer.instance);
                    inBuffer.instance = nullptr;
                }

                if (inBuffer.memory)
                {
                    inLogicalDevice.freeMemory(inBuffer.memory);
                    inBuffer.memory = nullptr;
                }
            }
        }
    }
}