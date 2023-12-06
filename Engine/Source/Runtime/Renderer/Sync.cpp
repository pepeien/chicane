#include "Sync.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Sync
			{
				void initSempahore(vk::Semaphore& allocator, vk::Device& inLogicalDevice)
				{
					vk::SemaphoreCreateInfo semaphoreInfo = {};
					semaphoreInfo.flags = vk::SemaphoreCreateFlags();

					allocator = inLogicalDevice.createSemaphore(semaphoreInfo);
				}

				void initFence(vk::Fence& allocator, vk::Device& inLogicalDevice)
				{
					vk::FenceCreateInfo fenceCreateInfo = {};
					fenceCreateInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

					allocator = inLogicalDevice.createFence(fenceCreateInfo);
				}
			}
		}
	}
}