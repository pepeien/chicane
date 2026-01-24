#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanSwapchainBundle
        {
        public:
            vk::SwapchainKHR         instance;
            std::vector<VulkanFrame> frames;
            vk::Format               colorFormat;
            vk::Format               depthFormat;
            vk::Extent2D             extent;
            vk::Extent2D             midPoints;
        };
    }
}