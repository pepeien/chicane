#include "Vertex.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                void initBuffer(Buffer& outBuffer, const BufferCreateInfo& inCreateInfo)
                {
                    vk::BufferCreateInfo bufferInfo;
                    bufferInfo.flags       = vk::BufferCreateFlags();
                    bufferInfo.size        = inCreateInfo.size;
                    bufferInfo.usage       = inCreateInfo.usage;
                    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

                    outBuffer.instance = inCreateInfo.logicalDevice.createBuffer(bufferInfo);

                    allocateBuffer(outBuffer, inCreateInfo);
                }

                void allocateBuffer(Buffer& outBuffer, const BufferCreateInfo& inCreateInfo)
                {
                    vk::MemoryRequirements memoryRequirements = inCreateInfo.logicalDevice.getBufferMemoryRequirements(outBuffer.instance);

                    vk::MemoryAllocateInfo memoryAlocateInfo;
                    memoryAlocateInfo.allocationSize  = memoryRequirements.size;
                    memoryAlocateInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                        inCreateInfo.physicalDevice,
                        memoryRequirements.memoryTypeBits,
                        inCreateInfo.memoryProperties
                    );

                    outBuffer.memory = inCreateInfo.logicalDevice.allocateMemory(memoryAlocateInfo);

                    inCreateInfo.logicalDevice.bindBufferMemory(outBuffer.instance, outBuffer.memory, 0);
                }

                void copyBuffer(
                    const Buffer& inSourceBuffer,
                    const Buffer& inDestinationBuffer,
                    const vk::DeviceSize& inAllocationSize,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                )
                {
                    inCommandBuffer.reset();

                    vk::CommandBufferBeginInfo commandBufferBegin;
                    commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

                    inCommandBuffer.begin(commandBufferBegin);

                    vk::BufferCopy copyRegion;
                    copyRegion.srcOffset = 0;
                    copyRegion.dstOffset = 0;
                    copyRegion.size      = inAllocationSize;

                    inCommandBuffer.copyBuffer(
                        inSourceBuffer.instance,
                        inDestinationBuffer.instance,
                        1,
                        &copyRegion
                    );

                    inCommandBuffer.end();

                    vk::SubmitInfo submitInfo;
                    submitInfo.commandBufferCount = 1;
                    submitInfo.pCommandBuffers    = &inCommandBuffer;

                    if (inQueue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess)
                    {
                        Core::Log::warning("Error while trying to copy the vertex buffer");
                    }

                    inQueue.waitIdle();
                }
            }
        }
    }
}