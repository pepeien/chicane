#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanDescriptorPoolSizeCreateInfo
        {
        public:
            using List = std::vector<VulkanDescriptorPoolSizeCreateInfo>;

        public:
            vk::DescriptorType type;
            std::uint32_t      descriptorCount;
        };
    }
}