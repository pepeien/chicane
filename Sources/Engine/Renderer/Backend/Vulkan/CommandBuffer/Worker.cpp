#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanCommandBufferWorker
        {
            void startJob(const vk::CommandBuffer& inCommandBuffer)
            {
                inCommandBuffer.reset();

                vk::CommandBufferBeginInfo commandBufferBegin;
                commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

                inCommandBuffer.begin(commandBufferBegin);
            }

            void endJob(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const String&            inDescription
            )
            {
                vk::SubmitInfo submitInfo;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers    = &inCommandBuffer;

                endJob(inQueue, submitInfo, inDescription);
            }

            void endJob(
                const vk::Queue& inQueue, const vk::SubmitInfo& inSubmitInfo, const String& inDescription
            )
            {
                for (std::uint32_t i = 0; i < inSubmitInfo.commandBufferCount; i++)
                {
                    inSubmitInfo.pCommandBuffers[i * sizeof(vk::CommandBuffer)].end();
                }

                vk::Result submitResult = inQueue.submit(1, &inSubmitInfo, nullptr);

                if (submitResult != vk::Result::eSuccess)
                {
                    throw std::runtime_error("Error while ending [" + inDescription + "]");
                }

                inQueue.waitIdle();
            }
        }
    }
}