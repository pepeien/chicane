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
                    size_t size;
                    vk::BufferUsageFlags usage;
                    vk::Device logicalDevice;
                    vk::PhysicalDevice physicalDevice;
                };

                struct Buffer
                {
                    vk::Buffer instance;
                    vk::DeviceMemory memory;
                };

                void initBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
                void allocateBuffer(Buffer& outBuffer, BufferCreateInfo& inCreateInfo);
            };
        }
    }
}