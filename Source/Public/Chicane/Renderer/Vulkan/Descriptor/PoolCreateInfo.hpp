#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct PoolCreateInfo
            {
            public:
                vk::DescriptorPoolCreateFlags   flags = vk::DescriptorPoolCreateFlags();
                std::uint32_t                   size  = 0;
                std::vector<vk::DescriptorType> types {};
            };
        }
    }
}