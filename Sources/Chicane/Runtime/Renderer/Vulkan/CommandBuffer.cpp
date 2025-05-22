#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer.hpp"

#include "Chicane/Runtime/Renderer/Vulkan/Frame.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CommandBuffer
        {
            void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo)
            {
                vk::CommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.commandPool        = inCreateInfo.commandPool;
                allocateInfo.level              = vk::CommandBufferLevel::ePrimary;
                allocateInfo.commandBufferCount = static_cast<std::uint32_t>(outCommandBuffers.size());

                outCommandBuffers = inCreateInfo.logicalDevice.allocateCommandBuffers(allocateInfo);
            }

            void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo)
            {
                vk::CommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.commandPool        = inCreateInfo.commandPool;
                allocateInfo.level              = vk::CommandBufferLevel::ePrimary;
                allocateInfo.commandBufferCount = 1;

                outCommandBuffer = inCreateInfo
                                   .logicalDevice
                                   .allocateCommandBuffers(allocateInfo)[0];
            }
        }
    }
}