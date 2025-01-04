#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct Bundle
            {
            public:
                vk::DescriptorSetLayout setLayout = {};
                vk::DescriptorSet       set       = {};
                vk::DescriptorPool      pool      = VK_NULL_HANDLE;
            };
        }
    }
}