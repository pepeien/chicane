#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Sync
			{
				void initSempahore(vk::Semaphore& allocator, vk::Device& inLogicalDevice);
				void initFence(vk::Fence& allocator, vk::Device& inLogicalDevice);
			}
		}
	}
}