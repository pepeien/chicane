#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            struct CHICANE_RUNTIME Bundle
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