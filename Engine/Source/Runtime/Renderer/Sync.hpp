#pragma once

#include "Base.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Sync
			{
				void initSempahore(vk::Semaphore& allocator, vk::Device& inLogicalDevice);
				void initFence(vk::Fence& allocator, vk::Device& inLogicalDevice);
			}
		}
	}
}