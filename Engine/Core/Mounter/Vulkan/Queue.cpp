#include "Queue.h"

namespace Engine
{
	namespace Core
	{
		namespace Mounter
		{
			namespace Vulkan
			{
				void initGraphicsQueue(vk::Queue& allocator, vk::PhysicalDevice inPhysicalDevice, vk::Device inLogicalDevice)
				{
					Core::Vulkan::Queue::FamilyIndices familyIndices;
					Core::Vulkan::Queue::findFamilyInidices(familyIndices, inPhysicalDevice);

					allocator = inLogicalDevice.getQueue(familyIndices.graphicsFamily.value(), 0);
				}
			}
		}
	}
}