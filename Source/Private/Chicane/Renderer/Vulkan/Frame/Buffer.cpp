#include "Chicane/Renderer/Vulkan/Frame/Buffer.hpp"

#include "Chicane/Renderer/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Vulkan/GraphicsPipeline.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            namespace Buffer
            {
                void init(Instance& outFrame, const CreateInfo& inCreateInfo)
                {
                    vk::FramebufferCreateInfo framebufferInfo {};
                    framebufferInfo.flags           = vk::FramebufferCreateFlags();
                    framebufferInfo.renderPass      = inCreateInfo.renderPass;
                    framebufferInfo.attachmentCount = static_cast<std::uint32_t>(inCreateInfo.attachments.size());
                    framebufferInfo.pAttachments    = inCreateInfo.attachments.data();
                    framebufferInfo.width           = inCreateInfo.swapChainExtent.width;
                    framebufferInfo.height          = inCreateInfo.swapChainExtent.height;
                    framebufferInfo.layers          = 1;

                    vk::Framebuffer frameBuffer = inCreateInfo.logicalDevice.createFramebuffer(framebufferInfo);
                    outFrame.addFrameBuffer(inCreateInfo.id, frameBuffer);
                }

                void initCommand(
                    std::vector<Instance>& outFrames,
                    const CommandBuffer::CreateInfo& inCreateInfo
                )
                {
                    for (int i = 0; i < outFrames.size(); i++)
                    {
                        initCommand(outFrames[i], inCreateInfo);
                    }
                }

                void initCommand(
                    Instance& outFrame,
                    const CommandBuffer::CreateInfo& inCreateInfo
                )
                {
                    CommandBuffer::init(outFrame.commandBuffer, inCreateInfo);
                }
            }
        }
    }
}