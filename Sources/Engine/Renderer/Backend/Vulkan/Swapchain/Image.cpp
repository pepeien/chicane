#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Image.hpp"

#include "Chicane/Renderer/Backend/Vulkan/CommandBuffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sync.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void VulkanSwapchainImage::init()
        {
            setupSync();

            setupCameraData();
            setupLightData();
            setup2DData();
            setup3DData();
        }

        void VulkanSwapchainImage::wait()
        {
            vk::Result result = logicalDevice.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
            if (result != vk::Result::eSuccess && result != vk::Result::eTimeout)
            {
                throw std::runtime_error("Error while waiting the fences");
            }

            result = logicalDevice.resetFences(1, &fence);
            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while resetting the fences");
            }

            commandBuffer.reset();
        }

        vk::ResultValue<std::uint32_t> VulkanSwapchainImage::acquire(const vk::SwapchainKHR& inSwapchain)
        {
            return logicalDevice.acquireNextImageKHR(inSwapchain, UINT64_MAX, imageAvailableSemaphore, nullptr);
        }

        void VulkanSwapchainImage::begin(const Frame& inFrame)
        {
            updateCameraData(inFrame.getCamera());
            updateLightData(inFrame.getLights());
            update2DData(inFrame.getInstances2D());
            update3DData(inFrame.getInstances3D());

            vk::CommandBufferBeginInfo commandBufferBegin;
            commandBufferBegin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
            commandBuffer.begin(commandBufferBegin);
        }

        void VulkanSwapchainImage::end()
        {
            commandBuffer.end();
        }

        void VulkanSwapchainImage::destroy()
        {
            for (auto& [id, frambuffer] : framebuffers)
            {
                logicalDevice.destroyFramebuffer(frambuffer);
            }

            destroyColorImage();
            destroyDepthImage();
            destroyShadowImage();
            destroyCameraData();
            destroyLightData();
            destroy2DData();
            destroy3DData();
            destroySync();
        }

        void VulkanSwapchainImage::setupCommandBuffer(const vk::CommandPool& inPool)
        {
            VulkanCommandBuffer::init(commandBuffer, {logicalDevice, inPool});
        }

        void VulkanSwapchainImage::addBuffer(const VulkanFrameCreateInfo& inCreateInfo)
        {
            vk::FramebufferCreateInfo framebufferInfo;
            framebufferInfo.renderPass      = inCreateInfo.renderPass;
            framebufferInfo.attachmentCount = static_cast<std::uint32_t>(inCreateInfo.attachments.size());
            framebufferInfo.pAttachments    = inCreateInfo.attachments.data();
            framebufferInfo.width           = inCreateInfo.extent.width;
            framebufferInfo.height          = inCreateInfo.extent.height;
            framebufferInfo.layers          = 1;

            vk::Framebuffer frameBuffer = inCreateInfo.logicalDevice.createFramebuffer(framebufferInfo);
            addFramebuffer(inCreateInfo.id, frameBuffer);
        }

        void VulkanSwapchainImage::setupSync()
        {
            VulkanSync::initSempahore(imageAvailableSemaphore, logicalDevice);
            VulkanSync::initSempahore(renderFinishedSemaphore, logicalDevice);
            VulkanSync::initFence(fence, logicalDevice);
        }

        void VulkanSwapchainImage::destroySync()
        {
            logicalDevice.destroyFence(fence);
            logicalDevice.destroySemaphore(imageAvailableSemaphore);
            logicalDevice.destroySemaphore(renderFinishedSemaphore);
        }

        void VulkanSwapchainImage::setupCameraData()
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = sizeof(View);
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;

            cameraResource.setup(bufferCreateInfo);
        }

        void VulkanSwapchainImage::updateCameraData(const View& inData)
        {
            View data = inData;
            data.flipY();

            cameraResource.copyToBuffer(&data, sizeof(View));
        }

        void VulkanSwapchainImage::destroyCameraData()
        {
            cameraResource.destroy(logicalDevice);
        }

        void VulkanSwapchainImage::setupLightData()
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = sizeof(View);
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;

            lightResource.setup(bufferCreateInfo);
        }

        void VulkanSwapchainImage::updateLightData(const View::List& inData)
        {
            if (inData.empty())
            {
                return;
            }

            View data = inData.at(0);
            data.flipY();

            lightResource.copyToBuffer(&data, sizeof(View));
        }

        void VulkanSwapchainImage::destroyLightData()
        {
            lightResource.destroy(logicalDevice);
        }

        void VulkanSwapchainImage::setup2DData()
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = sizeof(DrawPoly2DInstance) * 1000;
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;

            poly2DResource.setup(bufferCreateInfo);
        }

        void VulkanSwapchainImage::update2DData(const DrawPoly2DInstance::List& inData)
        {
            if (inData.empty())
            {
                return;
            }

            poly2DResource.copyToBuffer(inData.data(), sizeof(DrawPoly2DInstance) * inData.size());
        }

        void VulkanSwapchainImage::destroy2DData()
        {
            poly2DResource.destroy(logicalDevice);
        }

        void VulkanSwapchainImage::setup3DData()
        {
            VulkanBufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.logicalDevice  = logicalDevice;
            bufferCreateInfo.physicalDevice = physicalDevice;
            bufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            bufferCreateInfo.size  = sizeof(DrawPoly3DInstance) * 1000;
            bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;

            poly3DResource.setup(bufferCreateInfo);
        }

        void VulkanSwapchainImage::update3DData(const DrawPoly3DInstance::List& inData)
        {
            if (inData.empty())
            {
                return;
            }

            poly3DResource.copyToBuffer(inData.data(), sizeof(DrawPoly3DInstance) * inData.size());
        }

        void VulkanSwapchainImage::destroy3DData()
        {
            poly3DResource.destroy(logicalDevice);
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

        void VulkanSwapchainImage::setupShadowImage(vk::Format inFormat, const vk::Extent2D& inExtent)
        {
            shadowImage.format = inFormat;
            shadowImage.extent = inExtent;

            VulkanImageCreateInfo instanceCreateInfo = {};
            instanceCreateInfo.width                 = shadowImage.extent.width;
            instanceCreateInfo.height                = shadowImage.extent.height;
            instanceCreateInfo.count                 = 1;
            instanceCreateInfo.tiling                = vk::ImageTiling::eOptimal;
            instanceCreateInfo.flags                 = vk::ImageCreateFlagBits();
            instanceCreateInfo.usage =
                vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
            instanceCreateInfo.format        = inFormat;
            instanceCreateInfo.logicalDevice = logicalDevice;
            VulkanImage::initInstance(shadowImage.instance, instanceCreateInfo);

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
            samplerCreateInfo.unnormalizedCoordinates = false;
            samplerCreateInfo.compareEnable           = true;
            samplerCreateInfo.compareOp               = vk::CompareOp::eLess;
            samplerCreateInfo.minLod                  = 0.0f;
            samplerCreateInfo.maxLod                  = 1.0f;
            samplerCreateInfo.unnormalizedCoordinates = false;
            shadowImage.sampler                       = logicalDevice.createSampler(samplerCreateInfo);

            VulkanImageMemoryCreateInfo memoryCreateInfo = {};
            memoryCreateInfo.properties                  = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice               = logicalDevice;
            memoryCreateInfo.physicalDevice              = physicalDevice;
            VulkanImage::initMemory(shadowImage.memory, shadowImage.instance, memoryCreateInfo);

            VulkanImageViewCreateInfo viewCreateInfo = {};
            viewCreateInfo.count                     = instanceCreateInfo.count;
            viewCreateInfo.type                      = vk::ImageViewType::e2D;
            viewCreateInfo.aspect                    = vk::ImageAspectFlagBits::eDepth;
            viewCreateInfo.format                    = instanceCreateInfo.format;
            viewCreateInfo.logicalDevice             = logicalDevice;
            VulkanImage::initView(shadowImage.view, shadowImage.instance, viewCreateInfo);

            shadowImageInfo.imageLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
            shadowImageInfo.imageView   = shadowImage.view;
            shadowImageInfo.sampler     = shadowImage.sampler;
        }

        void VulkanSwapchainImage::destroyShadowImage()
        {
            logicalDevice.freeMemory(shadowImage.memory);
            logicalDevice.destroyImage(shadowImage.instance);
            logicalDevice.destroyImageView(shadowImage.view);
            logicalDevice.destroySampler(shadowImage.sampler);
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

        void VulkanSwapchainImage::addDescriptorSet(const String& inId, const vk::DescriptorSet& inDescriptorSet)
        {
            if (descriptorSets.find(inId) != descriptorSets.end())
            {
                throw std::runtime_error("Descriptor set " + inId + " already exists");
            }

            descriptorSets.insert(std::make_pair(inId, inDescriptorSet));
        }

        vk::DescriptorSet VulkanSwapchainImage::getDescriptorSet(const String& inId) const
        {
            return descriptorSets.at(inId);
        }

        void VulkanSwapchainImage::addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet)
        {
            descriptorSetWrites.push_back(inWriteDescriptorSet);

            updateDescriptorSets();
        }

        void VulkanSwapchainImage::updateDescriptorSets()
        {
            if (descriptorSetWrites.empty())
            {
                return;
            }

            logicalDevice.updateDescriptorSets(descriptorSetWrites, nullptr);
        }
    }
}