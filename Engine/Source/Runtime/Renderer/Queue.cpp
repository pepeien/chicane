#include "Queue.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Queue
			{
				bool FamilyIndices::isComplete()
                {
                    return graphicsFamily.has_value() && presentFamily.has_value();
                }

                void findFamilyInidices(
                    FamilyIndices& outFamilyIndices,
                    vk::PhysicalDevice& inDevice,
                    vk::SurfaceKHR& inSurface
                )
                {
                    FamilyIndices nextFamilyIndices;

                    std::vector<vk::QueueFamilyProperties> queueFamilies = inDevice.getQueueFamilyProperties();
    
                    for (int i = 0; i < queueFamilies.size(); i++)
                    {
                        vk::QueueFamilyProperties queueFamily = queueFamilies[i];

                        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
                        {
                            nextFamilyIndices.graphicsFamily = i;
                        }

                        if (inDevice.getSurfaceSupportKHR(i, inSurface))
                        {
                            nextFamilyIndices.presentFamily  = i;
                        }

                        if (nextFamilyIndices.isComplete())
                        {
                            outFamilyIndices = nextFamilyIndices;

                            return;
                        }
                    }

                    throw std::runtime_error("Couldn't pick queue families");
                }

				void initGraphicsQueue(
					vk::Queue& outQueue,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
					FamilyIndices familyIndices;
					findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					outQueue = inLogicalDevice.getQueue(familyIndices.graphicsFamily.value(), 0);
				}

				void initPresentQueue(
					vk::Queue& outQueue,
					vk::PhysicalDevice& inPhysicalDevice,
					vk::Device& inLogicalDevice,
					vk::SurfaceKHR& inSurface
				)
				{
					FamilyIndices familyIndices;
					findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);

					outQueue = inLogicalDevice.getQueue(familyIndices.presentFamily.value(), 0);
				}
			}
		}
	}
}