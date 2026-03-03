#include "Chicane/Renderer/Backend/Vulkan/Texture.hpp"

#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        VulkanTexture::VulkanTexture(const VulkanTextureCreateInfo& inCreateInfo)
            : m_logicalDevice(inCreateInfo.logicalDevice),
              m_physicalDevice(inCreateInfo.physicalDevice),
              m_commandBuffer(inCreateInfo.commandBuffer),
              m_queue(inCreateInfo.queue)
        {
            initExtent(inCreateInfo.image);
            initInstance();
            initSampler();
            initMemory();
            initView();
            copyPixels(inCreateInfo.image);
        }

        VulkanTexture::~VulkanTexture()
        {
            m_logicalDevice.freeMemory(memory);
            m_logicalDevice.destroyImage(instance);
            m_logicalDevice.destroyImageView(view);
            m_logicalDevice.destroySampler(sampler);
        }

        void VulkanTexture::initExtent(const Image::Reference inImage)
        {
            if (const Image::Instance instance = inImage.lock())
            {
                extent.width  = static_cast<std::uint32_t>(instance->getWidth());
                extent.height = static_cast<std::uint32_t>(instance->getHeight());
            }
        }

        void VulkanTexture::initInstance()
        {
            VulkanImageCreateInfo createInfo;
            createInfo.width         = extent.width;
            createInfo.height        = extent.height;
            createInfo.count         = 1;
            createInfo.tiling        = vk::ImageTiling::eOptimal;
            createInfo.usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            createInfo.format        = vk::Format::eR8G8B8A8Unorm;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initInstance(instance, createInfo);
        }

        void VulkanTexture::initSampler()
        {
            VulkanImageSamplerCreateInfo createInfo;
            createInfo.addressMode   = vk::SamplerAddressMode::eRepeat;
            createInfo.borderColor   = vk::BorderColor::eIntTransparentBlack;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initSampler(sampler, createInfo);
        }

        void VulkanTexture::initMemory()
        {
            VulkanImageMemoryCreateInfo createInfo;
            createInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
            createInfo.logicalDevice  = m_logicalDevice;
            createInfo.physicalDevice = m_physicalDevice;
            VulkanImage::initMemory(memory, instance, createInfo);
        }

        void VulkanTexture::initView()
        {
            VulkanImageViewCreateInfo createInfo;
            createInfo.count         = 1;
            createInfo.type          = vk::ImageViewType::e2D;
            createInfo.aspect        = vk::ImageAspectFlagBits::eColor;
            createInfo.format        = vk::Format::eR8G8B8A8Unorm;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initView(view, instance, createInfo);
        }

        void VulkanTexture::copyPixels(const Image::Reference inImage)
        {
            VulkanBufferCreateInfo createInfo;
            createInfo.logicalDevice  = m_logicalDevice;
            createInfo.physicalDevice = m_physicalDevice;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
            createInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.size  = 0;

            if (const Image::Instance instance = inImage.lock())
            {
                createInfo.size = instance->getMemorySize();
            }

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);
            if (const Image::Instance instance = inImage.lock())
            {
                void* statingWriteLocation = m_logicalDevice.mapMemory(stagingBuffer.memory, 0, createInfo.size);
                memcpy(statingWriteLocation, instance->getPixels(), createInfo.size);
                m_logicalDevice.unmapMemory(stagingBuffer.memory);
            }

            VulkanImage::transitionLayout(
                m_commandBuffer,
                m_queue,
                instance,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                1
            );

            VulkanImage::copyBufferToImage(
                m_commandBuffer,
                m_queue,
                stagingBuffer.instance,
                instance,
                extent.width,
                extent.height,
                1
            );

            VulkanImage::transitionLayout(
                m_commandBuffer,
                m_queue,
                instance,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                1
            );

            stagingBuffer.destroy(m_logicalDevice);
        }
    }
}