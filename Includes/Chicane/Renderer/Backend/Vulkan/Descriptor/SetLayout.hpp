#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout/BidingsCreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDescriptorSetLayout
        {
            CHICANE_RENDERER void init(
                vk::DescriptorSetLayout&                          outDescriptorSetLayout,
                const vk::Device&                                 inLogicalDevice,
                const VulkanDescriptorSetLayoutBidingsCreateInfo& inBidingsCreateInfo
            );

            CHICANE_RENDERER void allocate(
                vk::DescriptorSet&             outDescriptorSet,
                const vk::Device&              inLogicalDevice,
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool&      inPool
            );
        }
    }
}