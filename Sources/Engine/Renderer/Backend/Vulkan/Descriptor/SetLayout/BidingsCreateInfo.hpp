#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanDescriptorSetLayoutBidingsCreateInfo
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