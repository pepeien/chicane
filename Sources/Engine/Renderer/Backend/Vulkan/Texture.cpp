#include "Chicane/Renderer/Backend/Vulkan/Texture.hpp"

#include <algorithm>
#include <cstring>
#include <vector>

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
              m_queue(inCreateInfo.queue),
              m_sourceWidth(1),
              m_sourceHeight(1),
              m_residentMinMip(0),
              m_mipLevels(1)
        {
            initExtent(inCreateInfo);
            initInstance();
            initSampler();
            initMemory();
            initView();
            copyPixels(inCreateInfo);
        }

        VulkanTexture::~VulkanTexture()
        {
            m_logicalDevice.freeMemory(memory);
            m_logicalDevice.destroyImage(instance);
            m_logicalDevice.destroyImageView(view);
            m_logicalDevice.destroySampler(sampler);
        }

        bool VulkanTexture::matches(const DrawTexture& inTexture) const
        {
            return m_sourceWidth == std::max(1u, inTexture.width) &&
                   m_sourceHeight == std::max(1u, inTexture.height) && m_residentMinMip == inTexture.residentMinMip;
        }

        void VulkanTexture::initExtent(const VulkanTextureCreateInfo& inCreateInfo)
        {
            m_residentMinMip = inCreateInfo.residentMinMip;
            m_sourceWidth    = 1;
            m_sourceHeight   = 1;

            if (inCreateInfo.texture)
            {
                m_sourceWidth  = std::max(1u, inCreateInfo.texture->width);
                m_sourceHeight = std::max(1u, inCreateInfo.texture->height);
                m_residentMinMip = inCreateInfo.texture->residentMinMip;
            }
            else if (const Image::Instance image = inCreateInfo.image.lock())
            {
                m_sourceWidth  = static_cast<std::uint32_t>(std::max(1, image->getWidth()));
                m_sourceHeight = static_cast<std::uint32_t>(std::max(1, image->getHeight()));
            }

            m_sourceWidth  = std::min(m_sourceWidth, TEXTURE_MAX_SIZE);
            m_sourceHeight = std::min(m_sourceHeight, TEXTURE_MAX_SIZE);

            extent.width  = std::max(1u, Image::mipDimension(m_sourceWidth, m_residentMinMip));
            extent.height = std::max(1u, Image::mipDimension(m_sourceHeight, m_residentMinMip));
            m_mipLevels   = Image::mipCount(extent.width, extent.height);
        }

        void VulkanTexture::initInstance()
        {
            VulkanImageCreateInfo createInfo;
            createInfo.width         = extent.width;
            createInfo.height        = extent.height;
            createInfo.count         = 1;
            createInfo.mipLevels     = m_mipLevels;
            createInfo.tiling        = vk::ImageTiling::eOptimal;
            createInfo.usage         = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                               vk::ImageUsageFlagBits::eSampled;
            createInfo.format        = vk::Format::eR8G8B8A8Unorm;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initInstance(instance, createInfo);
        }

        void VulkanTexture::initSampler()
        {
            const vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();
            const vk::PhysicalDeviceFeatures   features   = m_physicalDevice.getFeatures();

            VulkanImageSamplerCreateInfo createInfo;
            createInfo.mipLevels     = m_mipLevels;
            createInfo.bAnisotropy   = features.samplerAnisotropy;
            createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
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
            createInfo.mipLevels     = m_mipLevels;
            createInfo.type          = vk::ImageViewType::e2D;
            createInfo.aspect        = vk::ImageAspectFlagBits::eColor;
            createInfo.format        = vk::Format::eR8G8B8A8Unorm;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initView(view, instance, createInfo);
        }

        void VulkanTexture::copyPixels(const VulkanTextureCreateInfo& inCreateInfo)
        {
            const Image::MipChain* chain = inCreateInfo.texture && inCreateInfo.texture->mips
                                               ? inCreateInfo.texture->mips.get()
                                               : nullptr;

            std::uint32_t uploaded = 0;
            for (std::uint32_t gpuLevel = 0; gpuLevel < m_mipLevels; gpuLevel++)
            {
                const std::uint32_t sourceLevel = m_residentMinMip + gpuLevel;
                Image::Instance     image;

                if (chain)
                {
                    image = chain->decode(sourceLevel);
                }
                else if (gpuLevel == 0)
                {
                    image = inCreateInfo.image.lock();
                }

                const std::uint32_t levelWidth  = Image::mipDimension(extent.width, gpuLevel);
                const std::uint32_t levelHeight = Image::mipDimension(extent.height, gpuLevel);
                const vk::DeviceSize size =
                    static_cast<vk::DeviceSize>(levelWidth) * levelHeight * 4;

                if (!image || !image->getPixels())
                {
                    break;
                }

                VulkanBufferCreateInfo bufferCreateInfo;
                bufferCreateInfo.logicalDevice  = m_logicalDevice;
                bufferCreateInfo.physicalDevice = m_physicalDevice;
                bufferCreateInfo.memoryProperties =
                    vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
                bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
                bufferCreateInfo.size  = size;

                VulkanBuffer stagingBuffer;
                stagingBuffer.init(bufferCreateInfo);

                void* writeLocation = m_logicalDevice.mapMemory(stagingBuffer.memory, 0, bufferCreateInfo.size);
                if (image->getWidth() == static_cast<int>(levelWidth) &&
                    image->getHeight() == static_cast<int>(levelHeight) && image->getPixels())
                {
                    std::memcpy(writeLocation, image->getPixels(), static_cast<std::size_t>(bufferCreateInfo.size));
                }
                else
                {
                    std::vector<Image::Pixel> resized(static_cast<std::size_t>(bufferCreateInfo.size));
                    image->blit(resized.data(), static_cast<int>(levelWidth), static_cast<int>(levelHeight));
                    std::memcpy(writeLocation, resized.data(), static_cast<std::size_t>(bufferCreateInfo.size));
                }
                m_logicalDevice.unmapMemory(stagingBuffer.memory);

                if (gpuLevel == 0)
                {
                    VulkanImage::transitionLayout(
                        m_commandBuffer,
                        m_queue,
                        instance,
                        vk::ImageLayout::eUndefined,
                        vk::ImageLayout::eTransferDstOptimal,
                        1,
                        m_mipLevels
                    );
                }

                VulkanImage::copyBufferToImage(
                    m_commandBuffer,
                    m_queue,
                    stagingBuffer.instance,
                    instance,
                    levelWidth,
                    levelHeight,
                    1,
                    gpuLevel
                );

                stagingBuffer.destroy(m_logicalDevice);
                uploaded++;
            }

            if (uploaded == 0)
            {
                VulkanImage::transitionLayout(
                    m_commandBuffer,
                    m_queue,
                    instance,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    1,
                    m_mipLevels
                );

                return;
            }

            if (uploaded < m_mipLevels)
            {
                VulkanImage::generateMipmaps(
                    m_commandBuffer,
                    m_queue,
                    instance,
                    extent.width,
                    extent.height,
                    1,
                    m_mipLevels
                );

                return;
            }

            VulkanImage::transitionLayout(
                m_commandBuffer,
                m_queue,
                instance,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                1,
                m_mipLevels
            );
        }
    }
}
