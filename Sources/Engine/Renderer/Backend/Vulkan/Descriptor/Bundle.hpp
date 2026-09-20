#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanDescriptorBundle
        {
        public:
            vk::DescriptorSetLayout setLayout = {};
            vk::DescriptorSet       set       = {};
            vk::DescriptorPool      pool      = VK_NULL_HANDLE;
        };
    }
}