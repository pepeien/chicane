#include "Worker.hpp"

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
                inCommandBuffer.end();
            
                vk::SubmitInfo submitInfo;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers    = &inCommandBuffer;
            
                if (
                    inQueue.submit(
                        1,
                        &submitInfo,
                        nullptr
                    ) != vk::Result::eSuccess
                )
                {
                    LOG_WARNING("Error while ending [" + inDescription + "]");
                }
            
                inQueue.waitIdle();
            }
        }
    }
}