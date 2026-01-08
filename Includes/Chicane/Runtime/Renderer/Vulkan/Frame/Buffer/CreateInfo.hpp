#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/String.hpp"
#include "Chicane/Runtime.hpp"

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
                    String                     id;
                    vk::Device                 logicalDevice;
                    vk::RenderPass             renderPass;
                    vk::Extent2D               extent;
                    std::vector<vk::ImageView> attachments;
                };
            }
        }
    }
}