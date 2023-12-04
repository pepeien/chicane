#pragma once

#include "Base.h"

namespace Engine
{
	namespace Core
	{
		namespace Vulkan
		{
			namespace Mounter
			{
				void initInstance(vk::Instance& allocator, vk::DispatchLoaderDynamic& dldiAllocator);
			}
		}
	}
}