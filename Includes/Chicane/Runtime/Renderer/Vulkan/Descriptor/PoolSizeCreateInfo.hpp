#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE_RUNTIME PoolSizeCreateInfo
            {
            public:
                vk::DescriptorType type;
                std::uint32_t      descriptorCount;
            };
        }
    }
}