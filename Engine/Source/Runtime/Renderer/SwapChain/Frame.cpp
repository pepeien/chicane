#include "Frame.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace SwapChain
            {
				void Frame::initResources(vk::Device& inLogicalDevice, vk::PhysicalDevice& inPhysicalDevice)
                {
                    Vertex::BufferCreateInfo cameraBufferCreateInfo;
                    cameraBufferCreateInfo.logicalDevice    = inLogicalDevice;
                    cameraBufferCreateInfo.physicalDevice   = inPhysicalDevice;
                    cameraBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
                    cameraBufferCreateInfo.size             = sizeof(Uniform::BufferObject);
                    cameraBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eUniformBuffer;

                    Vertex::initBuffer(cameraDataBuffer, cameraBufferCreateInfo);

                    cameraDataWriteLocation = inLogicalDevice.mapMemory(
                        cameraDataBuffer.memory,
                        0,
                        cameraBufferCreateInfo.size
                    );

                    uniformDescriptorBufferInfo.buffer = cameraDataBuffer.instance;
                    uniformDescriptorBufferInfo.offset = 0;
                    uniformDescriptorBufferInfo.range  = sizeof(Uniform::BufferObject);
                }

                void Frame::writeDescriptorSet(vk::Device& inLogicalDevice)
                {
                    vk::WriteDescriptorSet writeInfo;

                    writeInfo.dstSet          = descriptorSet;
                    writeInfo.dstBinding      = 0;
                    writeInfo.dstArrayElement = 0;
                    writeInfo.descriptorCount = 1;
                    writeInfo.descriptorType  = vk::DescriptorType::eUniformBuffer;
                    writeInfo.pBufferInfo     = &uniformDescriptorBufferInfo;

                    inLogicalDevice.updateDescriptorSets(writeInfo, nullptr);
                }

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