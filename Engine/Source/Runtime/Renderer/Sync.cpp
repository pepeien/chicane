#include "Sync.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Sync
			{
				void initSempahore(vk::Semaphore& outSemaphore, vk::Device& inLogicalDevice)
				{
					vk::SemaphoreCreateInfo semaphoreInfo = {};
					semaphoreInfo.flags = vk::SemaphoreCreateFlags();

					outSemaphore = inLogicalDevice.createSemaphore(semaphoreInfo);
				}

				void initFence(vk::Fence& outFence, vk::Device& inLogicalDevice)
				{
					vk::FenceCreateInfo fenceCreateInfo = {};
					fenceCreateInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

					outFence = inLogicalDevice.createFence(fenceCreateInfo);
				}
			}
		}
	}
}