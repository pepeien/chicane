#include "Queue.h"

namespace Engine
{
    namespace Core
    {
        namespace Vulkan
        {
            namespace Queue
            {
                bool FamilyIndices::isComplete()
                {
                    return graphicsFamily.has_value() && presentFamily.has_value();
                }

                void findFamilyInidices(
                    FamilyIndices& allocator,
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
                            allocator = nextFamilyIndices;

                            return;
                        }
                    }

                    throw std::runtime_error("Couldn't pick queue families");
                }
            }
        }
    }
}