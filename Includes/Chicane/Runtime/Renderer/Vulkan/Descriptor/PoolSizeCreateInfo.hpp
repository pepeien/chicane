#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE PoolSizeCreateInfo
            {
            public:
                vk::DescriptorType type;
                std::uint32_t      descriptorCount;
            };
        }
    }
}