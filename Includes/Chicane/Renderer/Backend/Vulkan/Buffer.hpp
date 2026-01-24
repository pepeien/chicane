#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanBuffer
        {
        public:
            VulkanBuffer();

        public:
            void init(const VulkanBufferCreateInfo& inCreateInfo);
            void allocate(const VulkanBufferCreateInfo& inCreateInfo);
            void copy(
                const VulkanBuffer&      inDestination,
                const vk::DeviceSize&    inAllocationSize,
                const vk::Queue&         inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );
            void destroy(const vk::Device& inLogicalDevice);

        public:
            vk::Buffer       instance;
            vk::DeviceMemory memory;
        };
    }
}