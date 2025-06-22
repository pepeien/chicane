#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Frame.hpp"

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