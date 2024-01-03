#include "Buffer.hpp"

namespace Chicane
{
    namespace Frame
    {
        namespace Buffer
        {
            void init(std::vector<Instance>& outFrames, const CreateInfo& inCreateInfo)
            {
                for (int i = 0; i < outFrames.size(); i++)
                {
                    init(outFrames[i], inCreateInfo);
                }
            }

            void init(Instance& outFrame, const CreateInfo& inCreateInfo)
            {
                std::vector<vk::ImageView> attachments = {
                    outFrame.imageView,
                    outFrame.depthImageView
                };

                vk::FramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.flags           = vk::FramebufferCreateFlags();
                framebufferInfo.renderPass      = inCreateInfo.renderPass;
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments    = attachments.data();
                framebufferInfo.width           = inCreateInfo.swapChainExtent.width;
                framebufferInfo.height          = inCreateInfo.swapChainExtent.height;
                framebufferInfo.layers          = 1;

                outFrame.framebuffers[GraphicsPipeline::Type::STANDARD] = inCreateInfo
                    .logicalDevice
                    .createFramebuffer(
                        framebufferInfo
                    );
            }

            void initCommand(
                std::vector<Instance>& outFrames,
                const CommandBuffer::Instance::CreateInfo& inCreateInfo
            )
            {
                for (int i = 0; i < outFrames.size(); i++)
                {
                    initCommand(outFrames[i], inCreateInfo);
                }
            }

            void initCommand(
                Instance& outFrame,
                const CommandBuffer::Instance::CreateInfo& inCreateInfo
            )
            {
                CommandBuffer::Instance::init(outFrame.commandBuffer, inCreateInfo);
            }
        }
    }
}