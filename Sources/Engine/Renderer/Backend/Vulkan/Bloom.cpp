#include "Chicane/Renderer/Backend/Vulkan/Bloom.hpp"

#include <algorithm>

#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Memory/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/View/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void VulkanBloom::setup(
            vk::Device          inLogicalDevice,
            vk::PhysicalDevice  inPhysicalDevice,
            vk::Format          inFormat,
            const vk::Extent2D& inExtent
        )
        {
            const vk::Extent2D bloomExtent = {std::max(1u, inExtent.width / 2u), std::max(1u, inExtent.height / 2u)};

            for (VulkanImageInfo& image : images)
            {
                image.format = inFormat;
                image.extent = bloomExtent;

                VulkanImageCreateInfo instanceCreateInfo;
                instanceCreateInfo.flags  = vk::ImageCreateFlagBits();
                instanceCreateInfo.width  = bloomExtent.width;
                instanceCreateInfo.height = bloomExtent.height;
                instanceCreateInfo.count  = 1;
                instanceCreateInfo.tiling = vk::ImageTiling::eOptimal;
                instanceCreateInfo.usage  = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format = inFormat;
                instanceCreateInfo.logicalDevice = inLogicalDevice;
                VulkanImage::initInstance(image.instance, instanceCreateInfo);

                vk::SamplerCreateInfo samplerCreateInfo;
                samplerCreateInfo.minFilter    = vk::Filter::eLinear;
                samplerCreateInfo.magFilter    = vk::Filter::eLinear;
                samplerCreateInfo.mipmapMode   = vk::SamplerMipmapMode::eLinear;
                samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
                samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
                samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
                samplerCreateInfo.maxLod       = 1.0f;
                image.sampler                  = inLogicalDevice.createSampler(samplerCreateInfo);

                VulkanImageMemoryCreateInfo memoryCreateInfo;
                memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice  = inLogicalDevice;
                memoryCreateInfo.physicalDevice = inPhysicalDevice;
                VulkanImage::initMemory(image.memory, image.instance, memoryCreateInfo);

                VulkanImageViewCreateInfo viewCreateInfo;
                viewCreateInfo.count         = 1;
                viewCreateInfo.type          = vk::ImageViewType::e2D;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
                viewCreateInfo.format        = inFormat;
                viewCreateInfo.logicalDevice = inLogicalDevice;
                VulkanImage::initView(image.view, image.instance, viewCreateInfo);
            }
        }

        void VulkanBloom::destroyFramebuffer(vk::Device inLogicalDevice, vk::Framebuffer& inFramebuffer)
        {
            if (!inFramebuffer)
            {
                return;
            }

            inLogicalDevice.destroyFramebuffer(inFramebuffer);
            inFramebuffer = nullptr;
        }

        vk::Framebuffer VulkanBloom::makeFramebuffer(
            vk::Device inLogicalDevice, vk::RenderPass inPass, vk::ImageView inView, vk::Extent2D inExtent
        )
        {
            vk::FramebufferCreateInfo info;
            info.renderPass      = inPass;
            info.attachmentCount = 1;
            info.pAttachments    = &inView;
            info.width           = inExtent.width;
            info.height          = inExtent.height;
            info.layers          = 1;

            return inLogicalDevice.createFramebuffer(info);
        }

        void VulkanBloom::destroy(vk::Device inLogicalDevice)
        {
            destroyFramebuffer(inLogicalDevice, extractFramebuffer);
            for (vk::Framebuffer& framebuffer : framebuffers)
            {
                destroyFramebuffer(inLogicalDevice, framebuffer);
            }
            destroyFramebuffer(inLogicalDevice, compositeFramebuffer);

            for (VulkanImageInfo& image : images)
            {
                if (image.sampler)
                {
                    inLogicalDevice.destroySampler(image.sampler);
                    image.sampler = nullptr;
                }
                if (image.view)
                {
                    inLogicalDevice.destroyImageView(image.view);
                    image.view = nullptr;
                }
                if (image.instance)
                {
                    inLogicalDevice.destroyImage(image.instance);
                    image.instance = nullptr;
                }
                if (image.memory)
                {
                    inLogicalDevice.freeMemory(image.memory);
                    image.memory = nullptr;
                }
            }
        }

        void VulkanBloom::rebuildFramebuffers(
            vk::Device             inLogicalDevice,
            vk::RenderPass         inExtractPass,
            vk::RenderPass         inBlurPass,
            vk::RenderPass         inCompositePass,
            const VulkanImageInfo& inColorImage
        )
        {
            destroyFramebuffer(inLogicalDevice, extractFramebuffer);
            for (vk::Framebuffer& framebuffer : framebuffers)
            {
                destroyFramebuffer(inLogicalDevice, framebuffer);
            }
            destroyFramebuffer(inLogicalDevice, compositeFramebuffer);

            extractFramebuffer =
                makeFramebuffer(inLogicalDevice, inExtractPass, images.at(0).view, images.at(0).extent);
            framebuffers.at(0) = makeFramebuffer(inLogicalDevice, inBlurPass, images.at(0).view, images.at(0).extent);
            framebuffers.at(1) = makeFramebuffer(inLogicalDevice, inBlurPass, images.at(1).view, images.at(1).extent);
            compositeFramebuffer =
                makeFramebuffer(inLogicalDevice, inCompositePass, inColorImage.view, inColorImage.extent);
        }
    }
}
