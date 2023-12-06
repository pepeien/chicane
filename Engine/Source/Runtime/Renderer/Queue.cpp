#include "Queue.hpp"

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
				)
				{
					FamilyIndices familyIndices;
					findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					allocator = inLogicalDevice.getQueue(familyIndices.graphicsFamily.value(), 0);
				}

				void initPresentQueue(
					vk::Queue& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
					FamilyIndices familyIndices;
					findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					allocator = inLogicalDevice.getQueue(familyIndices.presentFamily.value(), 0);
				}
			}
		}
	}
}