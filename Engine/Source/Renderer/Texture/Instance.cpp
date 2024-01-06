#include "Instance.hpp"

namespace Chicane
{
    namespace Texture
    {
        Instance::Instance(const CreateInfo& inCreateInfo)
        : m_width(0),
          m_height(0),
          m_filepath(
            FileSystem::getRelativePath(
                ENGINE_TEXTURES_DIR + inCreateInfo.data.filepath
            )
          ),
          m_logicalDevice(inCreateInfo.logicalDevice),
          m_physicalDevice(inCreateInfo.physicalDevice),
          m_commandBuffer(inCreateInfo.commandBuffer),
          m_queue(inCreateInfo.queue),
          m_descriptor({ inCreateInfo.descriptorSetLayout, nullptr, inCreateInfo.descriptorPool })
        {
            load();

            Image::CreateInfo imageCreateInfo;
            imageCreateInfo.width            = m_width;
            imageCreateInfo.height           = m_height;
            imageCreateInfo.count            = TEXTURE_IMAGE_COUNT;
            imageCreateInfo.logicalDevice    = m_logicalDevice;
            imageCreateInfo.physicalDevice   = m_physicalDevice;
            imageCreateInfo.tiling           = vk::ImageTiling::eOptimal;
            imageCreateInfo.usage            = vk::ImageUsageFlagBits::eTransferDst |
                                               vk::ImageUsageFlagBits::eSampled;
            imageCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
            imageCreateInfo.format           = vk::Format::eR8G8B8A8Unorm;

            Image::init(m_image.instance, imageCreateInfo);
            Image::initMemory(
                m_image.memory,
                imageCreateInfo,
                m_image.instance
            );

            populate();

            free(m_pixels);

            initView();
            initSampler();
            initDescriptorSet();
        }

        Instance::~Instance()
        {
            m_logicalDevice.freeMemory(m_image.memory);
            m_logicalDevice.destroyImage(m_image.instance);
            m_logicalDevice.destroyImageView(m_image.view);
            m_logicalDevice.destroySampler(m_image.sampler);
        }

        void Instance::bind(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::PipelineLayout& inPipelineLayout
        )
        {
            inCommandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                inPipelineLayout,
                1,
                m_descriptor.set,
                nullptr
            );
        }

        void Instance::load()
        {
            m_pixels = stbi_load(
                m_filepath.c_str(),
                &m_width,
                &m_height,
                &m_channels,
                STBI_rgb_alpha
            );

            if (m_pixels == nullptr)
            {
                throw std::runtime_error(
                    "Failed to load the pixels for " + m_filepath
                );
            }
        }

        void Instance::populate()
        {
            Buffer::CreateInfo stagingBufferCreateInfo;
            stagingBufferCreateInfo.logicalDevice    = m_logicalDevice;
            stagingBufferCreateInfo.physicalDevice   = m_physicalDevice;
            stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostCoherent |
                                                       vk::MemoryPropertyFlagBits::eHostVisible;
            stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
            stagingBufferCreateInfo.size             = sizeof(float) * (m_width * m_height);

            Buffer::Instance stagingBuffer;
            Buffer::init(stagingBuffer, stagingBufferCreateInfo);

            void* statingWriteLocation = m_logicalDevice.mapMemory(
                stagingBuffer.memory,
                0,
                stagingBufferCreateInfo.size
            );
            memcpy(statingWriteLocation, m_pixels, stagingBufferCreateInfo.size);
            m_logicalDevice.unmapMemory(stagingBuffer.memory);

            Image::transitionLayout(
                m_commandBuffer,
                m_queue,
                m_image.instance,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                TEXTURE_IMAGE_COUNT
            );

            Image::copyBufferToImage(
                m_commandBuffer,
                m_queue,
                stagingBuffer.instance,
                m_image.instance,
                m_width,
                m_height,
                TEXTURE_IMAGE_COUNT
            );

            Image::transitionLayout(
                m_commandBuffer,
                m_queue,
                m_image.instance,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                TEXTURE_IMAGE_COUNT
            );

            Buffer::destroy(m_logicalDevice, stagingBuffer);
        }

        void Instance::initView()
        {
            Image::initView(
                m_image.view,
                m_logicalDevice,
                m_image.instance,
                vk::Format::eR8G8B8A8Unorm,
                vk::ImageAspectFlagBits::eColor,
                vk::ImageViewType::e2D,
                TEXTURE_IMAGE_COUNT
            );
        }

        void Instance::initSampler()
        {
            vk::SamplerCreateInfo createInfo;
            createInfo.flags                   = vk::SamplerCreateFlags();
            createInfo.minFilter               = vk::Filter::eNearest;
            createInfo.magFilter               = vk::Filter::eLinear;
            createInfo.addressModeU            = vk::SamplerAddressMode::eRepeat;
            createInfo.addressModeV            = vk::SamplerAddressMode::eRepeat;
            createInfo.addressModeW            = vk::SamplerAddressMode::eRepeat;
            createInfo.anisotropyEnable        = false;
            createInfo.maxAnisotropy           = 1.0f;
            createInfo.borderColor             = vk::BorderColor::eIntTransparentBlack;
            createInfo.unnormalizedCoordinates = false;
            createInfo.compareEnable           = false;
            createInfo.compareOp               = vk::CompareOp::eAlways;
            createInfo.mipmapMode              = vk::SamplerMipmapMode::eLinear;
            createInfo.mipLodBias              = 0.0f;
            createInfo.minLod                  = 0.0f;
            createInfo.maxLod                  = 0.0f;

            m_image.sampler = m_logicalDevice.createSampler(createInfo);
        }

        void Instance::initDescriptorSet()
        {
            Descriptor::initSet(
                m_descriptor.set,
                m_logicalDevice,
                m_descriptor.setLayout,
                m_descriptor.pool
            );

            vk::DescriptorImageInfo imageDescriptorInfo;
            imageDescriptorInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageDescriptorInfo.imageView   = m_image.view;
            imageDescriptorInfo.sampler     = m_image.sampler;

            vk::WriteDescriptorSet imageWriteDescriptorSet;
            imageWriteDescriptorSet.dstSet          = m_descriptor.set;
            imageWriteDescriptorSet.dstBinding      = 0;
            imageWriteDescriptorSet.dstArrayElement = 0;
            imageWriteDescriptorSet.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
            imageWriteDescriptorSet.descriptorCount = 1;
            imageWriteDescriptorSet.pImageInfo      = &imageDescriptorInfo;

            m_logicalDevice.updateDescriptorSets(
                imageWriteDescriptorSet,
                nullptr
            );
        }
    }
}