#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer.hpp"

#include <cstdint>

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanCommandBuffer
        {
            void init(
                std::vector<vk::CommandBuffer>&      outCommandBuffers,
                const VulkanCommandBufferCreateInfo& inCreateInfo
            )
            {
                vk::CommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.commandPool                   = inCreateInfo.commandPool;
                allocateInfo.level                         = vk::CommandBufferLevel::ePrimary;
                allocateInfo.commandBufferCount = static_cast<std::uint32_t>(outCommandBuffers.size());

                outCommandBuffers = inCreateInfo.logicalDevice.allocateCommandBuffers(allocateInfo);
            }

            void init(vk::CommandBuffer& outCommandBuffer, const VulkanCommandBufferCreateInfo& inCreateInfo)
            {
                vk::CommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.commandPool                   = inCreateInfo.commandPool;
                allocateInfo.level                         = vk::CommandBufferLevel::ePrimary;
                allocateInfo.commandBufferCount            = 1;

                outCommandBuffer = inCreateInfo.logicalDevice.allocateCommandBuffers(allocateInfo)[0];
            }
        }
    }
}