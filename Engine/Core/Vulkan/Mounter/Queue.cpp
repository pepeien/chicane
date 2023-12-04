#include "Queue.h"

#include "Core/Vulkan/Queue.h"

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
				)
				{
					Vulkan::Queue::FamilyIndices familyIndices;
					Vulkan::Queue::findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					allocator = inLogicalDevice.getQueue(familyIndices.graphicsFamily.value(), 0);
				}

				void initPresentQueue(
					vk::Queue& allocator,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
					Vulkan::Queue::FamilyIndices familyIndices;
					Vulkan::Queue::findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					allocator = inLogicalDevice.getQueue(familyIndices.presentFamily.value(), 0);
				}
			}
		}
	}
}