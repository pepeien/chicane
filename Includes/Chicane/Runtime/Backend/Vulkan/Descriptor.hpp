#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor/PoolCreateInfo.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Descriptor/SetLayoutBidingsCreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            CHICANE_RUNTIME void initPool(
                vk::DescriptorPool& outDescriptorPool,
                const vk::Device& inLogicalDevice,
                const PoolCreateInfo& inCreateInfo
            );

            CHICANE_RUNTIME void initSetLayout(
                vk::DescriptorSetLayout& outDescriptorSetLayout,
                const vk::Device& inLogicalDevice,
                const SetLayoutBidingsCreateInfo& inBidingsCreateInfo
            );

            CHICANE_RUNTIME void allocalteSet(
                vk::DescriptorSet& outDescriptorSet,
                const vk::Device& inLogicalDevice,
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool& inPool
            );
        }
    }
}