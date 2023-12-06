#include "Command.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Command
			{
				void initPool(
					vk::CommandPool& allocator,
					vk::Device& inLogicalDevice,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
					Queue::FamilyIndices queueFamilyIndices;
					Queue::findFamilyInidices(queueFamilyIndices, inPhysicalDevice, inSurface);

					vk::CommandPoolCreateInfo poolInfo = {};
					poolInfo.flags            = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
					poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

					allocator = inLogicalDevice.createCommandPool(poolInfo);
				}

				void initBuffers(vk::CommandBuffer& allocator, Buffer::CommandBufferCreateInfo& createInfo)
				{
					vk::CommandBufferAllocateInfo allocateInfo = {};
					allocateInfo.commandPool        = createInfo.commandPool;
					allocateInfo.level              = vk::CommandBufferLevel::ePrimary;
					allocateInfo.commandBufferCount = 1;

					for (int i = 0; i < createInfo.frames.size(); i++)
					{
						createInfo.frames[i].commandBuffer = createInfo.logicalDevice.allocateCommandBuffers(allocateInfo)[0];
					}

					allocator = createInfo.logicalDevice.allocateCommandBuffers(allocateInfo)[0];
				}
			}
		}
	}
}