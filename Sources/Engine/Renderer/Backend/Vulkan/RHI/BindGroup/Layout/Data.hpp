#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/RHI/BindGroup/Layout/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanRHILayoutData
        {
        public:
            vk::DescriptorSetLayout        layout     = nullptr;
            RHI::BindGroupLayoutCreateInfo createInfo = {};
        };
    }
}
