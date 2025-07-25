#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor/PoolSizeCreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE_RUNTIME PoolCreateInfo
            {
            public:
                vk::DescriptorPoolCreateFlags   flags   = vk::DescriptorPoolCreateFlags();
                std::uint32_t                   maxSets = 0;
                std::vector<PoolSizeCreateInfo> sizes   = {};
            };
        }
    }
}