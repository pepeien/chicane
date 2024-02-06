#include "Renderer/CommandBuffer/Pool.hpp"

namespace Chicane
{
    namespace CommandBuffer
    {
        namespace Pool
        {
            void init(
                vk::CommandPool& outCommandPool,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            )
            {
                Queue::FamilyIndices queueFamilyIndices;
                Queue::findFamilyInidices(
                    queueFamilyIndices,
                    inPhysicalDevice,
                    inSurface
                );

                vk::CommandPoolCreateInfo poolInfo = {};
                poolInfo.flags            = vk::CommandPoolCreateFlags() |
                                            vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
                poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

                outCommandPool = inLogicalDevice.createCommandPool(poolInfo);
            }
        }
    }
}