#include "Runtime/Renderer/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace CommandBuffer
    {
        namespace Worker
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
                const vk::Queue& inQueue,
                const std::string& inDescription
            )
            {
                vk::SubmitInfo submitInfo;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers    = &inCommandBuffer;
            
                endJob(
                    inQueue,
                    submitInfo,
                    inDescription
                );
            }

            void endJob(
                const vk::Queue& inQueue,
                const vk::SubmitInfo& inSubmitInfo,
                const std::string& inDescription
            )
            {

                for (uint32_t i = 0; i < inSubmitInfo.commandBufferCount; i++)
                {
                    inSubmitInfo.pCommandBuffers[i * sizeof(vk::CommandBuffer)].end();
                }

                if (
                    inQueue.submit(
                        1,
                        &inSubmitInfo,
                        nullptr
                    ) != vk::Result::eSuccess
                )
                {
                    throw std::runtime_error("Error while ending [" + inDescription + "]");
                }

                inQueue.waitIdle();
            }
        }
    }
}