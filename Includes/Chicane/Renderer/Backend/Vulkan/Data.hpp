#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanBackendData
        {
        public:
            VulkanFrame       frame;
            vk::CommandBuffer commandBuffer;
            vk::Extent2D      swapChainExtent;
        };
    }
}