#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Descriptor/Bundle.hpp"
#include "Chicane/Renderer/Descriptor/PoolCreateInfo.hpp"
#include "Chicane/Renderer/Descriptor/SetLayoutBidingsCreateInfo.hpp"

namespace Chicane
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