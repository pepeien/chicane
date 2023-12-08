#include "Buffer.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Buffer
            {
                void initFramebuffers(FramebufferCreateInfo& outCreateInfo)
                {
                    for (int i = 0; i < outCreateInfo.frames.size(); i++)
                    {
                        std::vector<vk::ImageView> attachments = {
                            outCreateInfo.frames[i].imageView
                        };

                        vk::FramebufferCreateInfo framebufferInfo = {};
                        framebufferInfo.flags           = vk::FramebufferCreateFlags();
                        framebufferInfo.renderPass      = outCreateInfo.renderPass;
                        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                        framebufferInfo.pAttachments    = attachments.data();
                        framebufferInfo.width           = outCreateInfo.swapChainExtent.width;
                        framebufferInfo.height          = outCreateInfo.swapChainExtent.height;
                        framebufferInfo.layers          = 1;

                        outCreateInfo.frames[i].framebuffer = outCreateInfo.logicalDevice.createFramebuffer(framebufferInfo);
                    }
                }
            }
        }
    }
}