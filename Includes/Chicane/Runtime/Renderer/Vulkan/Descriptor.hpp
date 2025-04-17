#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/Descriptor/Bundle.hpp"
#include "Runtime/Renderer/Vulkan/Descriptor/PoolCreateInfo.hpp"
#include "Runtime/Renderer/Vulkan/Descriptor/SetLayoutBidingsCreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            CHICANE void initPool(
                vk::DescriptorPool& outDescriptorPool,
                const vk::Device& inLogicalDevice,
                const PoolCreateInfo& inCreateInfo
            );

            CHICANE void initSetLayout(
                vk::DescriptorSetLayout& outDescriptorSetLayout,
                const vk::Device& inLogicalDevice,
                const SetLayoutBidingsCreateInfo& inBidingsCreateInfo
            );

            CHICANE void allocalteSet(
                vk::DescriptorSet& outDescriptorSet,
                const vk::Device& inLogicalDevice,
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool& inPool
            );
        }
    }
}