#pragma once

#include "Base.hpp"

#include "Classes/Queue.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Device
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