#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            struct Bundle
            {
            public:
                vk::SwapchainKHR             instance;
                std::vector<Frame::Instance> frames;
                vk::Format                   format;
                vk::Format                   depthFormat;
                vk::Extent2D                 extent;
                vk::Extent2D                 midPoints;
            };
        }
    }
}