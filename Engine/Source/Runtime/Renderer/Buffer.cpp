#include "Buffer.hpp"

namespace Engine
{
	namespace Runtime
	{
		namespace Renderer
		{
			namespace Buffer
			{
				void initFramebuffers(FramebufferCreateInfo& createInfo)
				{
					for (int i = 0; i < createInfo.frames.size(); i++)
					{
						std::vector<vk::ImageView> attachments = {
							createInfo.frames[i].imageView
						};

						vk::FramebufferCreateInfo framebufferInfo = {};
						framebufferInfo.flags           = vk::FramebufferCreateFlags();
						framebufferInfo.renderPass      = createInfo.renderPass;
						framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
						framebufferInfo.pAttachments    = attachments.data();
						framebufferInfo.width           = createInfo.swapChainExtent.width;
						framebufferInfo.height          = createInfo.swapChainExtent.height;
						framebufferInfo.layers          = 1;

						createInfo.frames[i].framebuffer = createInfo.logicalDevice.createFramebuffer(framebufferInfo);
					}
				}
			}
		}
	}
}