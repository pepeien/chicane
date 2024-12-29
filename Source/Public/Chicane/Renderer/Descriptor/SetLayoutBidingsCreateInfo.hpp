#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Descriptor
    {
        struct SetLayoutBidingsCreateInfo
        {
        public:
            std::uint32_t                           count        = 0;
            std::vector<std::uint32_t>              indices      = {};
            std::vector<vk::DescriptorType>         types        = {};
            std::vector<std::uint32_t>              counts       = {};
            std::vector<vk::ShaderStageFlags>       stages       = {};
            std::vector<vk::DescriptorBindingFlags> bindingFlags = {};
        };
    }
}