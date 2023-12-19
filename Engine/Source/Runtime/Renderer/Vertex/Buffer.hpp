#pragma once

#include "Base.hpp"

#include "Renderer/Device.hpp"
#include "Renderer/Command/Worker.hpp"

namespace Engine
{
    namespace Vertex
    {
        namespace Buffer
        {
            struct CreateInfo
            {
                vk::DeviceSize size;
                vk::BufferUsageFlags usage;
                vk::Device logicalDevice;
                vk::PhysicalDevice physicalDevice;
                vk::MemoryPropertyFlags memoryProperties;
            };

            struct Instance
            {
                vk::Buffer instance;
                vk::DeviceMemory memory;
            };

            void init(Instance& outBuffer, const CreateInfo& inCreateInfo);
               void allocate(Instance& outBuffer, const CreateInfo& inCreateInfo);
               void copy(
                   const Instance& inSourceBuffer,
                   const Instance& inDestinationBuffer,
                   const vk::DeviceSize& inAllocationSize,
                   const vk::Queue& inQueue,
                   const vk::CommandBuffer& inCommandBuffer
               );
               void destroy(const vk::Device& inLogicalDevice, const Instance& inBuffer);
        }
    }
}