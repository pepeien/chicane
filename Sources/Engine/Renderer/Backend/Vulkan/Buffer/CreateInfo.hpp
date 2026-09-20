#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class VulkanAllocator;

        struct CHICANE_RENDERER VulkanBufferCreateInfo
        {
        public:
            vk::DeviceSize          size;
            vk::BufferUsageFlags    usage;
            vk::Device              logicalDevice;
            vk::PhysicalDevice      physicalDevice;
            vk::MemoryPropertyFlags memoryProperties;
            VulkanAllocator*        allocator = nullptr;
        };
    }
}