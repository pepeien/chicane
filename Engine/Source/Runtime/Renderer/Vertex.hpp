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

                void initBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
                void allocateBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
                void copyBuffer(
                    Buffer& inSourceBuffer,
                    Buffer& inDestinationBuffer,
                    vk::DeviceSize& inAllocationSize,
                    vk::Queue& inQueue,
                    vk::CommandBuffer& inCommandBuffer
                );
            };
        }
    }
}