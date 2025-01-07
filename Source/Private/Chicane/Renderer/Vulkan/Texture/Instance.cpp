#include "Chicane/Renderer/Vulkan/Texture/Instance.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Renderer/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            Instance::Instance(const CreateInfo& inCreateInfo)
                : m_image({}),
                m_logicalDevice(inCreateInfo.logicalDevice),
                m_physicalDevice(inCreateInfo.physicalDevice),
                m_commandBuffer(inCreateInfo.commandBuffer),
                m_queue(inCreateInfo.queue)
            {
                m_image.width    = inCreateInfo.image.width;
                m_image.height   = inCreateInfo.image.height;
                m_image.channels = inCreateInfo.image.channels;
                m_image.pixels   = inCreateInfo.image.pixels;

                Image::CreateInfo createInfo {};
                createInfo.width            = m_image.width;
                createInfo.height           = m_image.height;
                createInfo.count            = TEXTURE_IMAGE_COUNT;
                createInfo.logicalDevice    = m_logicalDevice;
                createInfo.physicalDevice   = m_physicalDevice;
                createInfo.tiling           = vk::ImageTiling::eOptimal;
                createInfo.usage            = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
                createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
                createInfo.format           = vk::Format::eR8G8B8A8Unorm;

                Image::init(m_image.instance, createInfo);
                Image::initMemory(
                    m_image.memory,
                    createInfo,
                    m_image.instance
                );

                populate();

                initView();
                initSampler();
            }

            Instance::~Instance()
            {
                m_logicalDevice.freeMemory(m_image.memory);
                m_logicalDevice.destroyImage(m_image.instance);
                m_logicalDevice.destroyImageView(m_image.view);
                m_logicalDevice.destroySampler(m_image.sampler);
            }

            const Image::Data& Instance::getImage() const
            {
                return m_image;
            }

            void Instance::populate()
            {
                Buffer::CreateInfo stagingBufferCreateInfo;
                stagingBufferCreateInfo.logicalDevice    = m_logicalDevice;
                stagingBufferCreateInfo.physicalDevice   = m_physicalDevice;
                stagingBufferCreateInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostCoherent |
                                                           vk::MemoryPropertyFlagBits::eHostVisible;
                stagingBufferCreateInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
                stagingBufferCreateInfo.size             = sizeof(float) * (m_image.width * m_image.height);

                Buffer::Instance stagingBuffer;
                Buffer::init(stagingBuffer, stagingBufferCreateInfo);

                void* statingWriteLocation = m_logicalDevice.mapMemory(
                    stagingBuffer.memory,
                    0,
                    stagingBufferCreateInfo.size
                );
                memcpy(statingWriteLocation, m_image.pixels, stagingBufferCreateInfo.size);
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
                    m_image.width,
                    m_image.height,
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
        }
    }
}