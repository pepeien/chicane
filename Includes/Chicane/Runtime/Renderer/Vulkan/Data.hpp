#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        struct CHICANE_RUNTIME RendererData
        {
        public:
            Frame::Instance   frame;
            vk::CommandBuffer commandBuffer;
            vk::Extent2D      swapChainExtent;
        };
    }
}