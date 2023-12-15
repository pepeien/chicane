#pragma once

#include "Base.hpp"

#include "Device.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Vertex
            {
                struct BufferCreateInfo
                {
                    vk::DeviceSize size;
                    vk::BufferUsageFlags usage;
                    vk::Device logicalDevice;
                    vk::PhysicalDevice physicalDevice;
                    vk::MemoryPropertyFlags memoryProperties;
                };

                struct Buffer
                {
                    vk::Buffer instance;
                    vk::DeviceMemory memory;
                };

                class Base
                {
                public:
                    glm::vec2 position;
                    glm::vec3 color;
                };

                void initBuffer(Buffer& outBuffer, const BufferCreateInfo& inCreateInfo);
                void allocateBuffer(Buffer& outBuffer, const BufferCreateInfo& inCreateInfo);
                void copyBuffer(
                    const Buffer& inSourceBuffer,
                    const Buffer& inDestinationBuffer,
                    const vk::DeviceSize& inAllocationSize,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );
            };
        }
    }
}