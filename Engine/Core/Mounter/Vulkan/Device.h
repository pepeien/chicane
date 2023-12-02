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
				void pickPhysicalDevice(vk::PhysicalDevice& allocator, vk::Instance& inInstance);
				void initLogicalDevice(vk::Device& allocator, vk::PhysicalDevice& inPhysicalDevice);
			}
		}
	}
}