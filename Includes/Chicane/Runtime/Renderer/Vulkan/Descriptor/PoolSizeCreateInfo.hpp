#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

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