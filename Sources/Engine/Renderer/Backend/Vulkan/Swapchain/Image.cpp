#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Image.hpp"

#include "Chicane/Renderer/Backend.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sync.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void VulkanSwapchainImage::destroy()
        {
            for (auto& [id, frambuffer] : framebuffers)
            {
                logicalDevice.destroyFramebuffer(frambuffer);
            }

            destroyColorImage();
            destroyDepthImage();
            destroySync();
        }

        void VulkanSwapchainImage::addBuffer(const VulkanFrameBufferCreateInfo& inCreateInfo)
        {
            vk::FramebufferCreateInfo createInfo;
            createInfo.renderPass      = inCreateInfo.renderPass;
            createInfo.attachmentCount = static_cast<std::uint32_t>(inCreateInfo.attachments.size());
            createInfo.pAttachments    = inCreateInfo.attachments.data();
            createInfo.width           = inCreateInfo.extent.width;
            createInfo.height          = inCreateInfo.extent.height;
            createInfo.layers          = 1;

            vk::Framebuffer frameBuffer = inCreateInfo.logicalDevice.createFramebuffer(createInfo);
            addFramebuffer(inCreateInfo.id, frameBuffer);
        }

        void VulkanSwapchainImage::setupSync()
        {
            VulkanSync::initSempahore(renderFineshedSemaphore, logicalDevice);
        }

        void VulkanSwapchainImage::destroySync()
        {
            logicalDevice.destroySemaphore(renderFineshedSemaphore);
        }

        void VulkanSwapchainImage::setupColorImage(vk::Format inFormat, const vk::Extent2D& inExtent)
        {
            colorImage.format = inFormat;
            colorImage.extent = inExtent;

            VulkanImageViewCreateInfo viewCreateInfo;
            viewCreateInfo.count         = 1;
            viewCreateInfo.type          = vk::ImageViewType::e2D;
            viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
            viewCreateInfo.format        = inFormat;
            viewCreateInfo.logicalDevice = logicalDevice;
            VulkanImage::initView(colorImage.view, colorImage.instance, viewCreateInfo);
        }

        void VulkanSwapchainImage::destroyColorImage()
        {
            logicalDevice.destroyImageView(colorImage.view);
        }

        void VulkanSwapchainImage::setupDepthImage(vk::Format inFormat, const vk::Extent2D& inExtent)
        {
            depthImage.format = inFormat;
            depthImage.extent = inExtent;

            VulkanImageCreateInfo instanceCreateInfo = {};
            instanceCreateInfo.flags                 = vk::ImageCreateFlagBits();
            instanceCreateInfo.width                 = depthImage.extent.width;
            instanceCreateInfo.height                = depthImage.extent.height;
            instanceCreateInfo.count                 = 1;
            instanceCreateInfo.tiling                = vk::ImageTiling::eOptimal;
            instanceCreateInfo.usage =
                vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
            instanceCreateInfo.format        = inFormat;
            instanceCreateInfo.logicalDevice = logicalDevice;
            VulkanImage::initInstance(depthImage.instance, instanceCreateInfo);

            vk::SamplerCreateInfo samplerCreateInfo   = {};
            samplerCreateInfo.flags                   = vk::SamplerCreateFlags();
            samplerCreateInfo.minFilter               = vk::Filter::eLinear;
            samplerCreateInfo.magFilter               = vk::Filter::eLinear;
            samplerCreateInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
            samplerCreateInfo.mipLodBias              = 0.0f;
            samplerCreateInfo.addressModeU            = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.addressModeV            = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.addressModeW            = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.anisotropyEnable        = false;
            samplerCreateInfo.maxAnisotropy           = 1.0f;
            samplerCreateInfo.borderColor             = vk::BorderColor::eFloatOpaqueWhite;
            samplerCreateInfo.compareEnable           = true;
            samplerCreateInfo.compareOp               = vk::CompareOp::eLess;
            samplerCreateInfo.minLod                  = 0.0f;
            samplerCreateInfo.maxLod                  = 1.0f;
            samplerCreateInfo.unnormalizedCoordinates = false;
            depthImage.sampler                        = logicalDevice.createSampler(samplerCreateInfo);

            VulkanImageMemoryCreateInfo memoryCreateInfo = {};
            memoryCreateInfo.properties                  = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice               = logicalDevice;
            memoryCreateInfo.physicalDevice              = physicalDevice;
            VulkanImage::initMemory(depthImage.memory, depthImage.instance, memoryCreateInfo);

            VulkanImageViewCreateInfo viewCreateInfo = {};
            viewCreateInfo.count                     = instanceCreateInfo.count;
            viewCreateInfo.type                      = vk::ImageViewType::e2D;
            viewCreateInfo.aspect                    = vk::ImageAspectFlagBits::eDepth;
            viewCreateInfo.format                    = instanceCreateInfo.format;
            viewCreateInfo.logicalDevice             = logicalDevice;
            VulkanImage::initView(depthImage.view, depthImage.instance, viewCreateInfo);
        }

        void VulkanSwapchainImage::destroyDepthImage()
        {
            logicalDevice.freeMemory(depthImage.memory);
            logicalDevice.destroyImage(depthImage.instance);
            logicalDevice.destroyImageView(depthImage.view);
            logicalDevice.destroySampler(depthImage.sampler);
        }

        void VulkanSwapchainImage::addFramebuffer(const String& inId, const vk::Framebuffer& inFramebuffer)
        {
            if (framebuffers.find(inId) != framebuffers.end())
            {
                throw std::runtime_error("Framebuffer " + inId + " already exists");
            }

            framebuffers.insert(std::make_pair(inId, inFramebuffer));
        }

        vk::Framebuffer VulkanSwapchainImage::getFramebuffer(const String& inId) const
        {
            return framebuffers.at(inId);
        }
    }
}