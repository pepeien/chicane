#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE_RUNTIME Bundle
            {
            public:
                vk::DescriptorSetLayout setLayout = {};
                vk::DescriptorSet       set       = {};
                vk::DescriptorPool      pool      = VK_NULL_HANDLE;
            };
        }
    }
}