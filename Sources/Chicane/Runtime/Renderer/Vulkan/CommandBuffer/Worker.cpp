#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer/Worker.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Vulkan
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
                    for (std::uint32_t i = 0; i < inSubmitInfo.commandBufferCount; i++)
                    {
                        inSubmitInfo.pCommandBuffers[i * sizeof(vk::CommandBuffer)].end();
                    }

                    vk::Result submitResult = inQueue.submit(
                        1,
                        &inSubmitInfo,
                        nullptr
                    );

                    if (submitResult != vk::Result::eSuccess)
                    {
                        throw std::runtime_error("Error while ending [" + inDescription + "]");
                    }

                    inQueue.waitIdle();
                }
            }
        }
    }
}