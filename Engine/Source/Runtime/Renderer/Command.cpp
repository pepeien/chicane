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
					vk::CommandPool& outCommandPool,
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

					outCommandPool = inLogicalDevice.createCommandPool(poolInfo);
				}

				void initBuffers(vk::CommandBuffer& outCommandBuffer, Buffer::CommandBufferCreateInfo& inCreateInfo)
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
}