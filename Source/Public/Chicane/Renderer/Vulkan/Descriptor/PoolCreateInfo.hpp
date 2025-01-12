#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor/PoolSizeCreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct PoolCreateInfo
            {
            public:
                vk::DescriptorPoolCreateFlags   flags   = vk::DescriptorPoolCreateFlags();
                std::uint32_t                   maxSets = 0;
                std::vector<PoolSizeCreateInfo> sizes   = {};
            };
        }
    }
}