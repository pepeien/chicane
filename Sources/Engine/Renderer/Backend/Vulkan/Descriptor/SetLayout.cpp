#include "Chicane/Renderer/Backend/Vulkan/Descriptor/SetLayout.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDescriptorSetLayout
        {
            void init(
                vk::DescriptorSetLayout&                          outDescriptorSetLayout,
                const vk::Device&                                 inLogicalDevice,
                const VulkanDescriptorSetLayoutBidingsCreateInfo& inBidingsCreateInfo
            )
            {
                std::vector<vk::DescriptorSetLayoutBinding> setLayoutBidings;
                setLayoutBidings.reserve(inBidingsCreateInfo.count);

                for (std::uint32_t i = 0; i < inBidingsCreateInfo.count; i++)
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
                    vk::DescriptorSetLayoutBindingFlagsCreateInfo bidingFlagsInfo{};
                    bidingFlagsInfo.bindingCount  = static_cast<std::uint32_t>(inBidingsCreateInfo.bindingFlags.size());
                    bidingFlagsInfo.pBindingFlags = inBidingsCreateInfo.bindingFlags.data();

                    setLayoutCreateInfo.pNext = &bidingFlagsInfo;
                }

                outDescriptorSetLayout = inLogicalDevice.createDescriptorSetLayout(setLayoutCreateInfo);
            }

            void allocate(
                vk::DescriptorSet&             outDescriptorSet,
                const vk::Device&              inLogicalDevice,
                const vk::DescriptorSetLayout& inLayout,
                const vk::DescriptorPool&      inPool
            )
            {
                vk::DescriptorSetAllocateInfo descriptorSetallocationInfo;
                descriptorSetallocationInfo.descriptorSetCount = 1;
                descriptorSetallocationInfo.descriptorPool     = inPool;
                descriptorSetallocationInfo.pSetLayouts        = &inLayout;

                outDescriptorSet = inLogicalDevice.allocateDescriptorSets(descriptorSetallocationInfo)[0];
            }
        }
    }
}