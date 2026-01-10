#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Descriptor/PoolSizeCreateInfo.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE_RUNTIME PoolCreateInfo
            {
            public:
                vk::DescriptorPoolCreateFlags flags =
                    vk::DescriptorPoolCreateFlags();
                std::uint32_t                   maxSets = 0;
                std::vector<PoolSizeCreateInfo> sizes   = {};
            };
        }
    }
}