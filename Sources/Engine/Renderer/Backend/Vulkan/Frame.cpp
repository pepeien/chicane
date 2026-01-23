#include "Chicane/Renderer/Backend/Vulkan/Frame.hpp"

#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/GraphicsPipeline.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Sync.hpp"
#include "Chicane/Runtime/Scene/Component/Camera.hpp"
#include "Chicane/Runtime/Scene/Component/Light.hpp"
#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Renderer
    {
        void VulkanFrame::wait(const vk::Device& inLogicalDevice)
        {
            vk::Result result = inLogicalDevice.waitForFences(1, &renderFence, VK_TRUE, UINT64_MAX);

            if (result != vk::Result::eSuccess && result != vk::Result::eTimeout)
            {
                throw std::runtime_error("Error while waiting the fences");
            }
        }

        void VulkanFrame::reset(const vk::Device& inLogicalDevice)
        {
            vk::Result result = inLogicalDevice.resetFences(1, &renderFence);

            if (result != vk::Result::eSuccess)
            {
                throw std::runtime_error("Error while resetting the fences");
            }
        }

        void VulkanFrame::destroy()
        {
            for (auto& [id, frambuffer] : framebuffers)
            {
                logicalDevice.destroyFramebuffer(frambuffer);
            }

            destroyColorImage();
            destroyDepthImage();
            destroyShadowImage();
            destroy2DData();
            destroy3DData();
            destroySync();
        }

        void VulkanFrame::addBuffer(const VulkanFrameCreateInfo& inCreateInfo)
        {
            vk::FramebufferCreateInfo framebufferInfo;
            framebufferInfo.renderPass      = inCreateInfo.renderPass;
            framebufferInfo.attachmentCount = static_cast<std::uint32_t>(inCreateInfo.attachments.size());
            framebufferInfo.pAttachments    = inCreateInfo.attachments.data();
            framebufferInfo.width           = inCreateInfo.extent.width;
            framebufferInfo.height          = inCreateInfo.extent.height;
            framebufferInfo.layers          = 1;

            vk::Framebuffer frameBuffer = inCreateInfo.logicalDevice.createFramebuffer(framebufferInfo);
            addFrameBuffer(inCreateInfo.id, frameBuffer);
        }

        void VulkanFrame::setupSync()
        {
            VulkanSync::initSempahore(presentSemaphore, logicalDevice);
            VulkanSync::initSempahore(renderSemaphore, logicalDevice);
            VulkanSync::initFence(renderFence, logicalDevice);
        }

        void VulkanFrame::destroySync()
        {
            logicalDevice.destroyFence(renderFence);
            logicalDevice.destroySemaphore(presentSemaphore);
            logicalDevice.destroySemaphore(renderSemaphore);
        }

        void VulkanFrame::setupCameraData()
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

        void VulkanFrame::updateCameraData(const View& inData)
        {
            if (cameraResource.isEmpty())
            {
                setupCameraData();
            }

            cameraResource.copyToBuffer(&inData, sizeof(View));
        }

        void VulkanFrame::destroyCameraData()
        {
            cameraResource.destroy(logicalDevice);
        }

        void VulkanFrame::setupLightData()
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

        void VulkanFrame::updateLightData(const View::List& inData)
        {
            if (inData.empty())
            {
                destroyLightData();

                return;
            }

            if (lightResource.isEmpty())
            {
                setupLightData();
            }

            lightResource.copyToBuffer(&inData.at(0), sizeof(View));
        }

        void VulkanFrame::destroyLightData()
        {
            lightResource.destroy(logicalDevice);
        }

        void VulkanFrame::setup2DData()
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

        void VulkanFrame::update2DData(const DrawPoly2DInstance::List& inData)
        {
            if (inData.empty())
            {
                destroy2DData();

                return;
            }

            if (poly2DResource.isEmpty())
            {
                setup2DData();
            }

            poly2DResource.copyToBuffer(inData.data(), sizeof(DrawPoly2DInstance) * inData.size());
        }

        void VulkanFrame::destroy2DData()
        {
            poly2DResource.destroy(logicalDevice);
        }

        void VulkanFrame::setup3DData()
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

        void VulkanFrame::update3DData(const DrawPoly3DInstance::List& inData)
        {
            if (inData.empty())
            {
                destroy3DData();

                return;
            }

            if (poly3DResource.isEmpty())
            {
                setup3DData();
            }

            poly3DResource.copyToBuffer(inData.data(), sizeof(DrawPoly3DInstance) * inData.size());
        }

        void VulkanFrame::destroy3DData()
        {
            poly3DResource.destroy(logicalDevice);
        }

        void VulkanFrame::setupColorImage(vk::Format inFormat, const vk::Extent2D& inExtent)
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

        void VulkanFrame::destroyColorImage()
        {
            logicalDevice.destroyImageView(colorImage.view);
        }

        void VulkanFrame::setupDepthImage(vk::Format inFormat, const vk::Extent2D& inExtent)
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
            samplerCreateInfo.unnormalizedCoordinates = false;
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

        void VulkanFrame::destroyDepthImage()
        {
            logicalDevice.freeMemory(depthImage.memory);
            logicalDevice.destroyImage(depthImage.instance);
            logicalDevice.destroyImageView(depthImage.view);
            logicalDevice.destroySampler(depthImage.sampler);
        }

        void VulkanFrame::setupShadowImage(vk::Format inFormat, const vk::Extent2D& inExtent)
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
            samplerCreateInfo.compareOp               = vk::CompareOp::eLessOrEqual;
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

        void VulkanFrame::destroyShadowImage()
        {
            logicalDevice.freeMemory(shadowImage.memory);
            logicalDevice.destroyImage(shadowImage.instance);
            logicalDevice.destroyImageView(shadowImage.view);
            logicalDevice.destroySampler(shadowImage.sampler);
        }

        void VulkanFrame::addFrameBuffer(const String& inId, const vk::Framebuffer& inFramebuffer)
        {
            if (framebuffers.find(inId) != framebuffers.end())
            {
                throw std::runtime_error("Framebuffer " + inId + " already exists");
            }

            framebuffers.insert(std::make_pair(inId, inFramebuffer));
        }

        vk::Framebuffer VulkanFrame::getFramebuffer(const String& inId) const
        {
            return framebuffers.at(inId);
        }

        void VulkanFrame::addDescriptorSet(const String& inId, const vk::DescriptorSet& inDescriptorSet)
        {
            if (descriptorSets.find(inId) != descriptorSets.end())
            {
                throw std::runtime_error("Descriptor set " + inId + " already exists");
            }

            descriptorSets.insert(std::make_pair(inId, inDescriptorSet));
        }

        vk::DescriptorSet VulkanFrame::getDescriptorSet(const String& inId) const
        {
            return descriptorSets.at(inId);
        }

        void VulkanFrame::addWriteDescriptorSet(const vk::WriteDescriptorSet& inWriteDescriptorSet)
        {
            descriptorSetWrites.push_back(inWriteDescriptorSet);
        }

        void VulkanFrame::updateDescriptorSets()
        {
            if (descriptorSetWrites.empty())
            {
                return;
            }

            logicalDevice.updateDescriptorSets(descriptorSetWrites, nullptr);
        }
    }
}