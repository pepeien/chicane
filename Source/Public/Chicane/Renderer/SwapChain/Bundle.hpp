#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Frame.hpp"

namespace Chicane
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