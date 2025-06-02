#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            namespace Buffer
            {
                struct CHICANE_RUNTIME CreateInfo
                {
                public:
                    std::string                id;
                    vk::Device                 logicalDevice;
                    vk::RenderPass             renderPass;
                    vk::Extent2D               extent;
                    std::vector<vk::ImageView> attachments;
                };
            }
        }
    }
}