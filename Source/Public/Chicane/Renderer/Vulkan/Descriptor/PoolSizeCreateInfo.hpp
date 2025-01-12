#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct PoolSizeCreateInfo
            {
            public:
                vk::DescriptorType type;
                std::uint32_t      descriptorCount;
            };
        }
    }
}