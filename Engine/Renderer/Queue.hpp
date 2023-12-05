#pragma once

#include "Base.hpp"

#include "Classes/Queue.hpp"

namespace Engine
{
	namespace Core
	{
		namespace Render
		{
			namespace Queue
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