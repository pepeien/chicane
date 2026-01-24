#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/SizeCreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanDescriptorPoolCreateInfo
        {
        public:
            vk::DescriptorPoolCreateFlags            flags   = vk::DescriptorPoolCreateFlags();
            std::uint32_t                            maxSets = 0;
            VulkanDescriptorPoolSizeCreateInfo::List sizes   = {};
        };
    }
}