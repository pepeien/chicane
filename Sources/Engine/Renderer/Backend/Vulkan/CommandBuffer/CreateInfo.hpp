#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanCommandBufferCreateInfo
        {
        public:
            vk::Device      logicalDevice;
            vk::CommandPool commandPool;
        };
    }
}