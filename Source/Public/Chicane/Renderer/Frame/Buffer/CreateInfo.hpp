#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Frame
    {
        namespace Buffer
        {
            struct CreateInfo
            {
                std::string                id;
                vk::Device                 logicalDevice;
                vk::RenderPass             renderPass;
                vk::Extent2D               swapChainExtent;
                std::vector<vk::ImageView> attachments;
            };
        }
    }
}