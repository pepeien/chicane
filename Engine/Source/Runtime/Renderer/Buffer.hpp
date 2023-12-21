#pragma once

#include "Base.hpp"

#include "Device.hpp"
#include "Command/Worker.hpp"

namespace Chicane
{
    namespace Buffer
    { 
        struct Instance
        {
            vk::Buffer instance;
            vk::DeviceMemory memory;
        };

        struct CreateInfo
        {
            vk::DeviceSize size;
            vk::BufferUsageFlags usage;
            vk::Device logicalDevice;
            vk::PhysicalDevice physicalDevice;
            vk::MemoryPropertyFlags memoryProperties;
        };
         
        void init(
            Instance& outBuffer,
            const CreateInfo& inCreateInfo
        );
        void allocate(
            Instance& outBuffer,
            const CreateInfo& inCreateInfo
        );
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