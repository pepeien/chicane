#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE Bundle
            {
            public:
                vk::DescriptorSetLayout setLayout = {};
                vk::DescriptorSet       set       = {};
                vk::DescriptorPool      pool      = VK_NULL_HANDLE;
            };
        }
    }
}