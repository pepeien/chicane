#include "Instance.hpp"

namespace Engine
{
    namespace CommandBuffer
    {
        namespace Instance
        {
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