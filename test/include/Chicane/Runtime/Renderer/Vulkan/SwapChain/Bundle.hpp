#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            struct CHICANE Bundle
            {
            public:
                vk::SwapchainKHR             instance;
                std::vector<Frame::Instance> frames;
                vk::Format                   colorFormat;
                vk::Format                   depthFormat;
                vk::Extent2D                 extent;
                vk::Extent2D                 midPoints;
            };
        }
    }
}