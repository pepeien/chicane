#include "Runtime/Renderer/Vulkan/Queue.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Queue
        {
            void findFamilyInidices(
                FamilyIndices& outFamilyIndices,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            )
            {
                FamilyIndices nextFamilyIndices;
        
                std::vector<vk::QueueFamilyProperties> queueFamilies = inPhysicalDevice
                                                                       .getQueueFamilyProperties();
            
                for (int i = 0; i < queueFamilies.size(); i++)
                {
                    vk::QueueFamilyProperties queueFamily = queueFamilies[i];
        
                    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
                    {
                        nextFamilyIndices.graphicsFamily = i;
                    }
        
                    if (inPhysicalDevice.getSurfaceSupportKHR(i, inSurface))
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
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            )
            {
                FamilyIndices familyIndices;
                findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);
        
                outQueue = inLogicalDevice.getQueue(
                    familyIndices.graphicsFamily.value(),
                    0
                );
            }
        
            void initPresentQueue(
                vk::Queue& outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            )
            {
                FamilyIndices familyIndices;
                findFamilyInidices(familyIndices, inPhysicalDevice, inSurface);
        
                outQueue = inLogicalDevice.getQueue(
                    familyIndices.presentFamily.value(),
                    0
                );
            }
        }
    }
}