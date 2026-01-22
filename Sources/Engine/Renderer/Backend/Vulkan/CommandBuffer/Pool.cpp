#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Pool.hpp"

#include "Chicane/Renderer/Backend/Vulkan/Queue.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanCommandBufferPool
        {
            void init(
                vk::CommandPool&          outCommandPool,
                const vk::Device&         inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR&     inSurface
            )
            {
                VulkanQueueFamilyIndices queueFamilyIndices(inPhysicalDevice, inSurface);

                vk::CommandPoolCreateInfo poolInfo = {};
                poolInfo.flags =
                    vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
                poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

                outCommandPool = inLogicalDevice.createCommandPool(poolInfo);
            }
        }
    }
}