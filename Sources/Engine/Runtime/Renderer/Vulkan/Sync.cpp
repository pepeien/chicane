#include "Chicane/Runtime/Renderer/Vulkan/Sync.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sync
        {
            void initSempahore(
                vk::Semaphore& outSemaphore, const vk::Device& inLogicalDevice
            )
            {
                vk::SemaphoreCreateInfo semaphoreInfo = {};
                semaphoreInfo.flags = vk::SemaphoreCreateFlags();

                outSemaphore = inLogicalDevice.createSemaphore(semaphoreInfo);
            }

            void
            initFence(vk::Fence& outFence, const vk::Device& inLogicalDevice)
            {
                vk::FenceCreateInfo fenceCreateInfo = {};
                fenceCreateInfo.flags =
                    vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

                outFence = inLogicalDevice.createFence(fenceCreateInfo);
            }
        }
    }
}