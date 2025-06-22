#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/SwapChain.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        struct CHICANE_RUNTIME RendererInternals
        {
        public:
            vk::PhysicalDevice physicalDevice;
            vk::Device         logicalDevice;
            vk::SurfaceKHR     sufrace;
            vk::Instance       instance;
            vk::Queue          graphicsQueue;
            vk::CommandBuffer  mainCommandBuffer;
            SwapChain::Bundle* swapchain;
            int                imageCount;
        };
    }
}