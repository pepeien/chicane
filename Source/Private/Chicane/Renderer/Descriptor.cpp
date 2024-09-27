#include "Chicane/Renderer/Descriptor.hpp"

namespace Chicane
{
    namespace Descriptor
    {
        void initPool(
            vk::DescriptorPool& outDescriptorPool,
            const vk::Device& inLogicalDevice,
            const PoolCreateInfo& inCreateInfo
        )
        {
            std::vector<vk::DescriptorPoolSize> poolSizes;
            
            for (uint32_t i = 0; i < inCreateInfo.types.size(); i++)
            {
                vk::DescriptorPoolSize poolSize;
                poolSize.type            = inCreateInfo.types[i];
                poolSize.descriptorCount = inCreateInfo.size;

                poolSizes.push_back(poolSize);
            }

            vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.flags         = inCreateInfo.flags;
            descriptorPoolCreateInfo.maxSets       = inCreateInfo.size;
            descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            descriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();

            outDescriptorPool = inLogicalDevice.createDescriptorPool(
                descriptorPoolCreateInfo
            );
        }
    
        void initSetLayout(
            vk::DescriptorSetLayout& outDescriptorSetLayout,
            const vk::Device& inLogicalDevice,
            const SetLayoutBidingsCreateInfo& inBidingsCreateInfo
        )
        {
            std::vector<vk::DescriptorSetLayoutBinding> setLayoutBidings;
            setLayoutBidings.reserve(inBidingsCreateInfo.count);

            for (uint32_t i = 0; i < inBidingsCreateInfo.count; i++)
            {
                vk::DescriptorSetLayoutBinding setLayoutBiding;
                setLayoutBiding.binding         = inBidingsCreateInfo.indices[i];
                setLayoutBiding.descriptorType  = inBidingsCreateInfo.types[i];
                setLayoutBiding.descriptorCount = inBidingsCreateInfo.counts[i];
                setLayoutBiding.stageFlags      = inBidingsCreateInfo.stages[i];

                setLayoutBidings.push_back(setLayoutBiding);
            }

            vk::DescriptorSetLayoutCreateInfo setLayoutCreateInfo;
            setLayoutCreateInfo.flags        = vk::DescriptorSetLayoutCreateFlags();
            setLayoutCreateInfo.bindingCount = static_cast<std::uint32_t>(setLayoutBidings.size());
            setLayoutCreateInfo.pBindings    = setLayoutBidings.data();

            if (!inBidingsCreateInfo.bindingFlags.empty())
            {
                vk::DescriptorSetLayoutBindingFlagsCreateInfo bidingFlagsInfo {};
                bidingFlagsInfo.bindingCount  = static_cast<std::uint32_t>(inBidingsCreateInfo.bindingFlags.size());
                bidingFlagsInfo.pBindingFlags = inBidingsCreateInfo.bindingFlags.data();

                setLayoutCreateInfo.pNext = &bidingFlagsInfo;
            }

            outDescriptorSetLayout = inLogicalDevice.createDescriptorSetLayout(
                setLayoutCreateInfo
            );
        }
    
        void allocalteSet(
            vk::DescriptorSet& outDescriptorSet,
            const vk::Device& inLogicalDevice,
            const vk::DescriptorSetLayout& inLayout,
            const vk::DescriptorPool& inPool
        )
        {
            vk::DescriptorSetAllocateInfo descriptorSetallocationInfo;
            descriptorSetallocationInfo.descriptorSetCount = 1;
            descriptorSetallocationInfo.descriptorPool     = inPool;
            descriptorSetallocationInfo.pSetLayouts        = &inLayout;

            outDescriptorSet = inLogicalDevice.allocateDescriptorSets(
                descriptorSetallocationInfo
            )[0];
        }
    }
}