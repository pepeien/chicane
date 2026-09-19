#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHIGroupData
        {
        public:
            vk::DescriptorPool      pool   = nullptr;
            vk::DescriptorSet       set    = nullptr;
            vk::DescriptorSetLayout layout = nullptr;
            bool                    bOwned = true;
        };
    }
}
