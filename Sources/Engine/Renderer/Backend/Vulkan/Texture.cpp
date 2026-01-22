#include "Chicane/Renderer/Backend/Vulkan/Texture.hpp"

#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanTexture::VulkanTexture(const VulkanTextureCreateInfo& inCreateInfo)
            : m_image(inCreateInfo.image),
              m_logicalDevice(inCreateInfo.logicalDevice),
              m_physicalDevice(inCreateInfo.physicalDevice),
              m_commandBuffer(inCreateInfo.commandBuffer),
              m_queue(inCreateInfo.queue)
        {
            VulkanImageCreateInfo instanceCreateInfo;
            instanceCreateInfo.width  = m_image.getWidth();
            instanceCreateInfo.height = m_image.getHeight();
            instanceCreateInfo.count  = 1;
            instanceCreateInfo.tiling = vk::ImageTiling::eOptimal;
            instanceCreateInfo.usage =
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            instanceCreateInfo.format        = vk::Format::eR8G8B8A8Unorm;
            instanceCreateInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initInstance(m_image.instance, instanceCreateInfo);

            VulkanImageSamplerCreateInfo samplerCreateInfo;
            samplerCreateInfo.addressMode   = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.borderColor   = vk::BorderColor::eIntTransparentBlack;
            samplerCreateInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initSampler(m_image.sampler, samplerCreateInfo);

            VulkanImageMemoryCreateInfo memoryCreateInfo;
            memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
            memoryCreateInfo.logicalDevice  = m_logicalDevice;
            memoryCreateInfo.physicalDevice = m_physicalDevice;
            VulkanImage::initMemory(m_image.memory, m_image.instance, memoryCreateInfo);

            VulkanImageViewCreateInfo viewCreateInfo;
            viewCreateInfo.count         = instanceCreateInfo.count;
            viewCreateInfo.type          = vk::ImageViewType::e2D;
            viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
            viewCreateInfo.format        = instanceCreateInfo.format;
            viewCreateInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initView(m_image.view, m_image.instance, viewCreateInfo);

            populate();
        }

        VulkanTexture::~VulkanTexture()
        {
            m_logicalDevice.freeMemory(m_image.memory);
            m_logicalDevice.destroyImage(m_image.instance);
            m_logicalDevice.destroyImageView(m_image.view);
            m_logicalDevice.destroySampler(m_image.sampler);
        }

        const VulkanImageData& VulkanTexture::getImage() const
        {
            return m_image;
        }

        void VulkanTexture::populate()
        {
            VulkanBufferCreateInfo stagingBufferCreateInfo;
            stagingBufferCreateInfo.logicalDevice  = m_logicalDevice;
            stagingBufferCreateInfo.physicalDevice = m_physicalDevice;
            stagingBufferCreateInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
            stagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferCreateInfo.size  = sizeof(float) * (m_image.getSize());

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(stagingBufferCreateInfo);

            void* statingWriteLocation =
                m_logicalDevice.mapMemory(stagingBuffer.memory, 0, stagingBufferCreateInfo.size);
            memcpy(statingWriteLocation, m_image.getPixels(), stagingBufferCreateInfo.size);
            m_logicalDevice.unmapMemory(stagingBuffer.memory);

            VulkanImage::transitionLayout(
                m_commandBuffer,
                m_queue,
                m_image.instance,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                1
            );

            VulkanImage::copyBufferToImage(
                m_commandBuffer,
                m_queue,
                stagingBuffer.instance,
                m_image.instance,
                m_image.getWidth(),
                m_image.getHeight(),
                1
            );

            VulkanImage::transitionLayout(
                m_commandBuffer,
                m_queue,
                m_image.instance,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                1
            );

            stagingBuffer.destroy(m_logicalDevice);
        }
    }
}