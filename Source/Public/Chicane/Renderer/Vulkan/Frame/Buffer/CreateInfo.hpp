#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            namespace Buffer
            {
                struct CreateInfo
                {
                public:
                    std::string                id;
                    vk::Device                 logicalDevice;
                    vk::RenderPass             renderPass;
                    vk::Extent2D               swapChainExtent;
                    std::vector<vk::ImageView> attachments;
                };
            }
        }
    }
}