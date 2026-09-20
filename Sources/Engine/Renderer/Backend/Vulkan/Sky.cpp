#include "Backend/Vulkan/Sky.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

#include "Backend/Vulkan/Allocator.hpp"
#include "Backend/Vulkan/Buffer.hpp"
#include "Backend/Vulkan/Image.hpp"
#include "Backend/Vulkan/Descriptor/SetLayout.hpp"

namespace Chicane
{
    namespace Renderer
    {
        static float srgbToLinear(float inValue)
        {
            if (inValue <= 0.04045f)
            {
                return inValue / 12.92f;
            }

            return std::pow((inValue + 0.055f) / 1.055f, 2.4f);
        }

        static std::uint16_t floatToHalf(float inValue)
        {
            union
            {
                float         f;
                std::uint32_t u;
            } bits{inValue};

            const std::uint32_t sign = (bits.u >> 16u) & 0x8000u;
            std::int32_t        exp  = static_cast<std::int32_t>((bits.u >> 23u) & 0xFFu) - 127 + 15;
            std::uint32_t       mant = bits.u & 0x7FFFFFu;

            if (exp <= 0)
            {
                return static_cast<std::uint16_t>(sign);
            }

            if (exp >= 31)
            {
                return static_cast<std::uint16_t>(sign | 0x7C00u);
            }

            return static_cast<std::uint16_t>(sign | (static_cast<std::uint32_t>(exp) << 10u) | (mant >> 13u));
        }
        VulkanSky::VulkanSky(const VulkanSkyCreateInfo& inCreateInfo)
            : m_logicalDevice(inCreateInfo.logicalDevice),
              m_physicalDevice(inCreateInfo.physicalDevice),
              m_commandBuffer(inCreateInfo.commandBuffer),
              m_queue(inCreateInfo.queue),
              m_allocator(inCreateInfo.allocator),
              m_mipLevels(1),
              m_descriptor({})
        {
            m_descriptor.setLayout = inCreateInfo.descriptorSetLayout;
            m_descriptor.set       = nullptr;
            m_descriptor.pool      = inCreateInfo.descriptorPool;

            initExtent(inCreateInfo.images);
            initMipLevels();
            initInstance(inCreateInfo.images.size());
            initSampler();
            initView(inCreateInfo.images.size());
            copyPixels(inCreateInfo.images);
            initDescriptorSet();
        }

        VulkanSky::~VulkanSky()
        {
            if (sampler)
            {
                m_logicalDevice.destroySampler(sampler);
                sampler = nullptr;
            }
            if (view)
            {
                m_logicalDevice.destroyImageView(view);
                view = nullptr;
            }
            if (m_allocator)
            {
                m_allocator->destroyImage(*this);
            }
        }

        void VulkanSky::bind(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout)
        {
            inCommandBuffer
                .bindDescriptorSets(vk::PipelineBindPoint::eGraphics, inPipelineLayout, 1, m_descriptor.set, nullptr);
        }

        vk::DescriptorImageInfo VulkanSky::getDescriptorImageInfo() const
        {
            vk::DescriptorImageInfo info;
            info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            info.imageView   = view;
            info.sampler     = sampler;
            return info;
        }

        void VulkanSky::initExtent(const Image::References& inImages)
        {
            extent.width  = INT32_MAX;
            extent.height = INT32_MAX;

            for (const Image::Reference reference : inImages)
            {
                if (const Image::Instance instance = reference.lock())
                {
                    extent.width  = std::min(extent.width, static_cast<std::uint32_t>(instance->getWidth()));
                    extent.height = std::min(extent.height, static_cast<std::uint32_t>(instance->getHeight()));
                }
            }
        }

        void VulkanSky::initMipLevels()
        {
            m_mipLevels           = 1;
            std::uint32_t largest = std::max(extent.width, extent.height);
            while (largest > 1)
            {
                largest >>= 1;
                m_mipLevels++;
            }
        }

        void VulkanSky::initInstance(std::uint32_t inCount)
        {
            VulkanImageCreateInfo createInfo;
            createInfo.width     = extent.width;
            createInfo.height    = extent.height;
            createInfo.count     = inCount;
            createInfo.mipLevels = m_mipLevels;
            createInfo.tiling    = vk::ImageTiling::eOptimal;
            createInfo.flags     = vk::ImageCreateFlagBits::eCubeCompatible;
            createInfo.usage     = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc |
                               vk::ImageUsageFlagBits::eSampled;
            createInfo.format        = vk::Format::eR16G16B16A16Sfloat;
            createInfo.logicalDevice = m_logicalDevice;

            VulkanImageMemoryCreateInfo createMemoryInfo;
            createMemoryInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
            createMemoryInfo.logicalDevice  = m_logicalDevice;
            createMemoryInfo.physicalDevice = m_physicalDevice;
            createMemoryInfo.allocator      = m_allocator;
            VulkanImage::init(*this, createInfo, createMemoryInfo);
        }

        void VulkanSky::initSampler()
        {
            VulkanImageSamplerCreateInfo createInfo;
            createInfo.mipLevels     = m_mipLevels;
            createInfo.addressMode   = vk::SamplerAddressMode::eClampToEdge;
            createInfo.borderColor   = vk::BorderColor::eFloatTransparentBlack;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initSampler(sampler, createInfo);
        }

        void VulkanSky::initView(std::uint32_t inCount)
        {
            VulkanImageViewCreateInfo createInfo;
            createInfo.count         = inCount;
            createInfo.mipLevels     = m_mipLevels;
            createInfo.type          = vk::ImageViewType::eCube;
            createInfo.aspect        = vk::ImageAspectFlagBits::eColor;
            createInfo.format        = vk::Format::eR16G16B16A16Sfloat;
            createInfo.logicalDevice = m_logicalDevice;
            VulkanImage::initView(view, instance, createInfo);
        }

        void VulkanSky::copyPixels(const Image::References& inImages)
        {
            const std::uint32_t  faces = static_cast<std::uint32_t>(inImages.size());
            const vk::DeviceSize faceBytes =
                static_cast<vk::DeviceSize>(extent.width) * extent.height * sizeof(std::uint16_t) * 4u;

            VulkanBufferCreateInfo createInfo;
            createInfo.logicalDevice  = m_logicalDevice;
            createInfo.physicalDevice = m_physicalDevice;
            createInfo.allocator      = m_allocator;
            createInfo.memoryProperties =
                vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
            createInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.size  = faceBytes * faces;

            VulkanBuffer stagingBuffer;
            stagingBuffer.init(createInfo);
            void* writeLocation = stagingBuffer.map();

            std::vector<unsigned char> srgb(static_cast<std::size_t>(extent.width) * extent.height * 4u);
            std::vector<std::uint16_t> linear(static_cast<std::size_t>(extent.width) * extent.height * 4u);

            vk::DeviceSize offset = 0;
            for (const Image::Reference reference : inImages)
            {
                std::fill(linear.begin(), linear.end(), 0);

                if (const Image::Instance instance = reference.lock())
                {
                    instance->blit(srgb.data(), static_cast<int>(extent.width), static_cast<int>(extent.height));

                    for (std::size_t index = 0; index < linear.size(); index++)
                    {
                        const float channel = static_cast<float>(srgb[index]) / 255.0f;
                        const bool  bAlpha  = (index % 4u) == 3u;
                        linear[index]       = floatToHalf(bAlpha ? channel : srgbToLinear(channel));
                    }
                }

                std::memcpy(
                    static_cast<char*>(writeLocation) + static_cast<std::size_t>(offset),
                    linear.data(),
                    static_cast<std::size_t>(faceBytes)
                );

                offset += faceBytes;
            }

            VulkanImage::transitionLayout(
                m_commandBuffer,
                m_queue,
                instance,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                faces,
                m_mipLevels
            );

            VulkanImage::copyBufferToImage(
                m_commandBuffer,
                m_queue,
                stagingBuffer.instance,
                instance,
                extent.width,
                extent.height,
                faces
            );

            VulkanImage::generateMipmaps(
                m_commandBuffer,
                m_queue,
                instance,
                extent.width,
                extent.height,
                faces,
                m_mipLevels
            );

            stagingBuffer.destroy();
        }

        void VulkanSky::initDescriptorSet()
        {
            VulkanDescriptorSetLayout::allocate(
                m_descriptor.set,
                m_logicalDevice,
                m_descriptor.setLayout,
                m_descriptor.pool
            );

            vk::DescriptorImageInfo descriptorInfo;
            descriptorInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            descriptorInfo.imageView   = view;
            descriptorInfo.sampler     = sampler;

            vk::WriteDescriptorSet descriptorSetInfo;
            descriptorSetInfo.dstSet          = m_descriptor.set;
            descriptorSetInfo.dstBinding      = 0;
            descriptorSetInfo.dstArrayElement = 0;
            descriptorSetInfo.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            descriptorSetInfo.descriptorCount = 1;
            descriptorSetInfo.pImageInfo      = &descriptorInfo;

            m_logicalDevice.updateDescriptorSets(descriptorSetInfo, nullptr);
        }
    }
}