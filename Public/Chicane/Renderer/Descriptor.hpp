#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Descriptor
    {
        struct PoolCreateInfo
        {
            vk::DescriptorPoolCreateFlags flags   = vk::DescriptorPoolCreateFlags();
            uint32_t size                         = 0;
            std::vector<vk::DescriptorType> types = {};
        };

        struct SetLayoutBidingsCreateInfo
        {
            uint32_t count;
            std::vector<uint32_t> indices;
            std::vector<vk::DescriptorType> types;
            std::vector<uint32_t> counts;
            std::vector<vk::ShaderStageFlags> stages;
        };

        struct Bundle
        {
            vk::DescriptorSetLayout setLayout = {};
            vk::DescriptorSet set = {};
            vk::DescriptorPool pool = VK_NULL_HANDLE;
        };

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

        void initSet(
            vk::DescriptorSet& outDescriptorSet,
            const vk::Device& inLogicalDevice,
            const vk::DescriptorSetLayout& inLayout,
            const vk::DescriptorPool& inPool
        );
    }
}