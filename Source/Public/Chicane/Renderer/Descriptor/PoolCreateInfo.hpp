#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
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