#include "Vertex.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                void initBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo)
                {
                    vk::BufferCreateInfo bufferInfo;
                    bufferInfo.flags       = vk::BufferCreateFlags();
                    bufferInfo.size        = inCreateInfo.size;
                    bufferInfo.usage       = inCreateInfo.usage;
                    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

                    outBuffer.instance = inCreateInfo.logicalDevice.createBuffer(bufferInfo);

                    allocateBuffer(outBuffer, inCreateInfo);
                }

                void allocateBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo)
                {
                    vk::MemoryRequirements memoryRequirements = inCreateInfo.logicalDevice.getBufferMemoryRequirements(outBuffer.instance);

                    vk::MemoryAllocateInfo memoryAlocateInfo;
                    memoryAlocateInfo.allocationSize  = memoryRequirements.size;
                    memoryAlocateInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                        inCreateInfo.physicalDevice,
                        memoryRequirements.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostVisible
                    );

                    outBuffer.memory = inCreateInfo.logicalDevice.allocateMemory(memoryAlocateInfo);

                    inCreateInfo.logicalDevice.bindBufferMemory(outBuffer.instance, outBuffer.memory, 0);
                }
            }
        }
    }
}