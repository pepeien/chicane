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
				void pickPhysicalDevice(vk::PhysicalDevice& allocator, vk::Instance& inInstance);
				void initLogicalDevice(
					vk::Device& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::SurfaceKHR& inSurface
				);
			}
		}
	}
}