#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Descriptor/Bundle.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Descriptor/PoolCreateInfo.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Descriptor/SetLayoutBidingsCreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Descriptor
        {
            void initPool(
                vk::DescriptorPool& outDescriptorPool,
                const vk::Device& inLogicalDevice,
                const PoolCreateInfo& inCreateInfo
            );

            void initSetLayout(
                vk::DescriptorSetLayout& outDescriptorSetLayout,
                const vk::Device& inLogicalDevice,
                const SetLayoutBidingsCreateInfo& inBidingsCreateInfo
            );

            void allocalteSet(
                vk::DescriptorSet& outDescriptorSet,
                const vk::Device& inLogicalDevice,
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool& inPool
            );
        }
    }
}