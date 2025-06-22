#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            struct CHICANE_RUNTIME SetLayoutBidingsCreateInfo
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
}