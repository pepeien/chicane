#include "CommandJob.hpp"

namespace Engine
{
    namespace Command
    {
        namespace Job
        {
            void start(const vk::CommandBuffer& inCommandBuffer)
            {
                inCommandBuffer.reset();
            
                vk::CommandBufferBeginInfo commandBufferBegin;
                commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            
                inCommandBuffer.begin(commandBufferBegin);
            }
            
            void end(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue& inQueue,
                const std::string& inDescription
            )
            {
                inCommandBuffer.end();
            
                vk::SubmitInfo submitInfo;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers    = &inCommandBuffer;
            
                if (inQueue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess)
                {
                    Log::warning("Error while ending [" + inDescription + "]");
                }
            
                inQueue.waitIdle();
            }
        }
    }
}