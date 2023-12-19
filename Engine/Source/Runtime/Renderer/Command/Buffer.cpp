#include "Buffer.hpp"

namespace Engine
{
    namespace Command
    {
		namespace Buffer
		{
			void init(vk::CommandBuffer& outCommandBuffer, const CreateInfo& inCreateInfo)
			{
				vk::CommandBufferAllocateInfo allocateInfo = {};
        		allocateInfo.commandPool        = inCreateInfo.commandPool;
        		allocateInfo.level              = vk::CommandBufferLevel::ePrimary;
        		allocateInfo.commandBufferCount = 1;

        		for (int i = 0; i < inCreateInfo.frames.size(); i++)
        		{
        		    inCreateInfo.frames[i].commandBuffer = inCreateInfo.logicalDevice.allocateCommandBuffers(allocateInfo)[0];
        		}

        		outCommandBuffer = inCreateInfo.logicalDevice.allocateCommandBuffers(allocateInfo)[0];
			}
		}
	}
}