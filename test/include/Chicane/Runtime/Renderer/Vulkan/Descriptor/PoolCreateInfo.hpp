#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Descriptor/PoolSizeCreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE PoolCreateInfo
            {
            public:
                vk::DescriptorPoolCreateFlags   flags   = vk::DescriptorPoolCreateFlags();
                std::uint32_t                   maxSets = 0;
                std::vector<PoolSizeCreateInfo> sizes   = {};
            };
        }
    }
}