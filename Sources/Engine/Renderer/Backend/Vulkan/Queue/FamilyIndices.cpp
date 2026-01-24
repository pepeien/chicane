#include "Chicane/Renderer/Backend/Vulkan/Queue/FamilyIndices.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanQueueFamilyIndices::VulkanQueueFamilyIndices(
            const vk::PhysicalDevice& inPhysicalDevice, const vk::SurfaceKHR& inSurface
        )
        {
            std::vector<vk::QueueFamilyProperties> queueFamilies =
                inPhysicalDevice.getQueueFamilyProperties();

            for (int i = 0; i < queueFamilies.size(); i++)
            {
                vk::QueueFamilyProperties queueFamily = queueFamilies[i];

                if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    graphicsFamily = i;
                }

                if (inPhysicalDevice.getSurfaceSupportKHR(i, inSurface))
                {
                    presentFamily = i;
                }

                if (isComplete())
                {
                    return;
                }
            }

            throw std::runtime_error("Couldn't pick queue families");
        }

        bool VulkanQueueFamilyIndices::isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    }
}