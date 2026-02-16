#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Image.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanSwapchainBundle
        {
        public:
            vk::SwapchainKHR                  instance;
            std::vector<VulkanSwapchainImage> images            = {};
            std::uint32_t                     currentImageIndex = 0U;
            vk::Format                        colorFormat;
            vk::Format                        depthFormat;
            vk::Extent2D                      extent = {};
        };
    }
}