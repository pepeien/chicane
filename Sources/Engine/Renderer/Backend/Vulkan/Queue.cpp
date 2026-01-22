#include "Chicane/Renderer/Backend/Vulkan/Queue.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanQueue
        {
            void initGraphicsQueue(
                vk::Queue&                outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface
            )
            {
                VulkanQueueFamilyIndices familyIndices(inPhysicalDevice, inSurface);

                outQueue = inLogicalDevice.getQueue(familyIndices.graphicsFamily.value(), 0);
            }

            void initPresentQueue(
                vk::Queue&                outQueue,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface
            )
            {
                VulkanQueueFamilyIndices familyIndices(inPhysicalDevice, inSurface);

                outQueue = inLogicalDevice.getQueue(familyIndices.presentFamily.value(), 0);
            }
        }
    }
}