#include "Chicane/Renderer/Backend/Vulkan/Descriptor/Pool.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDescriptorPool
        {
            void init(
                vk::DescriptorPool&                   outDescriptorPool,
                const vk::Device&                     inLogicalDevice,
                const VulkanDescriptorPoolCreateInfo& inCreateInfo
            )
            {
                std::vector<vk::DescriptorPoolSize> poolSizes = {};

                for (const VulkanDescriptorPoolSizeCreateInfo& size : inCreateInfo.sizes)
                {
                    vk::DescriptorPoolSize poolSize = {};
                    poolSize.type                   = size.type;
                    poolSize.descriptorCount        = size.descriptorCount;

                    poolSizes.push_back(poolSize);
                }

                vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
                descriptorPoolCreateInfo.flags         = inCreateInfo.flags;
                descriptorPoolCreateInfo.maxSets       = inCreateInfo.maxSets;
                descriptorPoolCreateInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
                descriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();

                outDescriptorPool = inLogicalDevice.createDescriptorPool(descriptorPoolCreateInfo);
            }
        }
    }
}