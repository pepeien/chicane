#include "Instance.hpp"

namespace Chicane
{
    namespace CommandBuffer
    {
        namespace Instance
        {
            void init(std::vector<vk::CommandBuffer>& outCommandBuffers, const CreateInfo& inCreateInfo)
            {
                vk::CommandBufferAllocateInfo allocateInfo = {};
                allocateInfo.commandPool        = inCreateInfo.commandPool;
                allocateInfo.level              = vk::CommandBufferLevel::ePrimary;
                allocateInfo.commandBufferCount = static_cast<uint32_t>(outCommandBuffers.size());

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