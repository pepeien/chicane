#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDescriptorPool
        {
            CHICANE_RENDERER void init(
                vk::DescriptorPool&                   outDescriptorPool,
                const vk::Device&                     inLogicalDevice,
                const VulkanDescriptorPoolCreateInfo& inCreateInfo
            );
        }
    }
}