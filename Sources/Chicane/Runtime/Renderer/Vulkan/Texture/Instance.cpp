#include "Runtime/Renderer/Vulkan/Texture/Instance.hpp"

#include "Box/Asset/Texture.hpp"
#include "Core/FileSystem.hpp"
#include "Runtime/Renderer/Vulkan/Buffer.hpp"
#include "Runtime/Renderer/Vulkan/Descriptor.hpp"
#include "Runtime/Renderer/Vulkan/Texture/CreateInfo.hpp"

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

                Image::Instance::CreateInfo instanceCreateInfo = {};
                instanceCreateInfo.width         = m_image.width;
                instanceCreateInfo.height        = m_image.height;
                instanceCreateInfo.count         = 1;
                instanceCreateInfo.tiling        = vk::ImageTiling::eOptimal;
                instanceCreateInfo.usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format        = vk::Format::eR8G8B8A8Unorm;
                instanceCreateInfo.logicalDevice = m_logicalDevice;
                Image::initInstance(m_image.instance, instanceCreateInfo);

                Image::Sampler::CreateInfo samplerCreateInfo = {};
                samplerCreateInfo.addressMode   = vk::SamplerAddressMode::eRepeat;
                samplerCreateInfo.borderColor   = vk::BorderColor::eIntTransparentBlack;
                samplerCreateInfo.logicalDevice = m_logicalDevice;
                Image::initSampler(m_image.sampler, samplerCreateInfo);

                Image::Memory::CreateInfo memoryCreateInfo = {};
                memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice  = m_logicalDevice;
                memoryCreateInfo.physicalDevice = m_physicalDevice;
                Image::initMemory(m_image.memory, m_image.instance, memoryCreateInfo);

                Image::View::CreateInfo viewCreateInfo = {};
                viewCreateInfo.count         = instanceCreateInfo.count;
                viewCreateInfo.type          = vk::ImageViewType::e2D;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
                viewCreateInfo.format        = instanceCreateInfo.format;
                viewCreateInfo.logicalDevice = m_logicalDevice;
                Image::initView(m_image.view, m_image.instance, viewCreateInfo);

                populate();
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
                    1
                );

                Image::copyBufferToImage(
                    m_commandBuffer,
                    m_queue,
                    stagingBuffer.instance,
                    m_image.instance,
                    m_image.width,
                    m_image.height,
                    1
                );

                Image::transitionLayout(
                    m_commandBuffer,
                    m_queue,
                    m_image.instance,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    1
                );

                Buffer::destroy(m_logicalDevice, stagingBuffer);
            }
        }
    }
}