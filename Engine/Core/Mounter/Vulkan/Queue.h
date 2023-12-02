#pragma once

#include "Base.h"
#include "Core/Queue.h"

namespace Engine
{
	namespace Core
	{
		namespace Mounter
		{
			namespace Vulkan
			{
				void initGraphicsQueue(vk::Queue& allocator, vk::PhysicalDevice inPhysicalDevice, vk::Device inLogicalDevice);
			}
		}
	}
}