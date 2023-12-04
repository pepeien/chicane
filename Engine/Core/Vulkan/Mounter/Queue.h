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
				void initGraphicsQueue(
					vk::Queue& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				);

				void initPresentQueue(
					vk::Queue& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				);
			}
		}
	}
}