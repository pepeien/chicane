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
                // Sky
                std::vector<vk::ImageView> skyAttachments = {
                    outFrame.imageView
                };

                vk::FramebufferCreateInfo skyFramebufferInfo = {};
                skyFramebufferInfo.flags           = vk::FramebufferCreateFlags();
                skyFramebufferInfo.renderPass      = inCreateInfo.renderPasses.at(Layer::SKY);
                skyFramebufferInfo.attachmentCount = static_cast<uint32_t>(skyAttachments.size());
                skyFramebufferInfo.pAttachments    = skyAttachments.data();
                skyFramebufferInfo.width           = inCreateInfo.swapChainExtent.width;
                skyFramebufferInfo.height          = inCreateInfo.swapChainExtent.height;
                skyFramebufferInfo.layers          = 1;

                vk::Framebuffer skyFrameBuffer = inCreateInfo.logicalDevice.createFramebuffer(skyFramebufferInfo);
                outFrame.framebuffers[Layer::SKY] = skyFrameBuffer; // Used bracket insert due to vulkan not being able to access framebuffer mem addr

                // Scene
                std::vector<vk::ImageView> sceneAttachments = {
                    outFrame.imageView,
                    outFrame.depthImageView
                };

                vk::FramebufferCreateInfo sceneFramebufferInfo = {};
                sceneFramebufferInfo.flags           = vk::FramebufferCreateFlags();
                sceneFramebufferInfo.renderPass      = inCreateInfo.renderPasses.at(Layer::SCENE);
                sceneFramebufferInfo.attachmentCount = static_cast<uint32_t>(sceneAttachments.size());
                sceneFramebufferInfo.pAttachments    = sceneAttachments.data();
                sceneFramebufferInfo.width           = inCreateInfo.swapChainExtent.width;
                sceneFramebufferInfo.height          = inCreateInfo.swapChainExtent.height;
                sceneFramebufferInfo.layers          = 1;

                vk::Framebuffer sceneFrameBuffer = inCreateInfo.logicalDevice.createFramebuffer(sceneFramebufferInfo);
                outFrame.framebuffers[Layer::SCENE] = sceneFrameBuffer; // Used bracket insert due to vulkan not being able to access framebuffer mem addr
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