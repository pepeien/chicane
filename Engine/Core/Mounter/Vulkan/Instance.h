#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		namespace Mounter
		{
			namespace Vulkan
			{
				void initInstance(vk::Instance& allocator, vk::DispatchLoaderDynamic& dldiAllocator);
			}
		}
	}
}