#include "Chicane/Renderer/Vulkan/CubeMap/Instance.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap/Instance.hpp"
#include "Chicane/Renderer/Vulkan/Image.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace CubeMap
        {
            Instance::Instance(const CreateInfo& inCreateInfo)
                : m_image({}),
                m_images(inCreateInfo.images),
                m_logicalDevice(inCreateInfo.logicalDevice),
                m_physicalDevice(inCreateInfo.physicalDevice),
                m_commandBuffer(inCreateInfo.commandBuffer),
                m_queue(inCreateInfo.queue),
                m_descriptor({})
            {
                if (m_images.size() < Chicane::CubeMap::IMAGE_COUNT)
                {
                    throw new std::runtime_error("Every cube map must have " + std::to_string(Chicane::CubeMap::IMAGE_COUNT) + " images.");
                }

                m_descriptor.setLayout = inCreateInfo.descriptorSetLayout;
                m_descriptor.set       = nullptr;
                m_descriptor.pool      = inCreateInfo.descriptorPool;

                const Chicane::Image::Data& baseImage = m_images.at(Box::CubeMap::Side::Up);
                m_image.width    = baseImage.width;
                m_image.height   = baseImage.height;
                m_image.channels = baseImage.channels;

                initImage();
                copyPixels();
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

            void Instance::initImage()
            {
                Image::Instance::CreateInfo instanceCreateInfo {};
                instanceCreateInfo.width         = m_image.width;
                instanceCreateInfo.height        = m_image.height;
                instanceCreateInfo.count         = Chicane::CubeMap::IMAGE_COUNT;
                instanceCreateInfo.tiling        = vk::ImageTiling::eOptimal;
                instanceCreateInfo.flags         = vk::ImageCreateFlagBits::eCubeCompatible;
                instanceCreateInfo.usage         = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format        = vk::Format::eR8G8B8A8Unorm;
                instanceCreateInfo.logicalDevice = m_logicalDevice;
                Image::initInstance(m_image.instance, instanceCreateInfo);

                Image::Sampler::CreateInfo samplerCreateInfo {};
                samplerCreateInfo.addressMode   = vk::SamplerAddressMode::eRepeat;
                samplerCreateInfo.borderColor   = vk::BorderColor::eIntTransparentBlack;
                samplerCreateInfo.logicalDevice = m_logicalDevice;
                Image::initSampler(m_image.sampler, samplerCreateInfo);

                Image::Memory::CreateInfo memoryCreateInfo {};
                memoryCreateInfo.properties     = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice  = m_logicalDevice;
                memoryCreateInfo.physicalDevice = m_physicalDevice;
                Image::initMemory(m_image.memory, m_image.instance, memoryCreateInfo);

                Image::View::CreateInfo viewCreateInfo {};
                viewCreateInfo.count         = instanceCreateInfo.count;
                viewCreateInfo.type          = vk::ImageViewType::eCube;
                viewCreateInfo.aspect        = vk::ImageAspectFlagBits::eColor;
                viewCreateInfo.format        = instanceCreateInfo.format;
                viewCreateInfo.logicalDevice = m_logicalDevice;
                Image::initView(m_image.view, m_image.instance, viewCreateInfo);
            }

            void Instance::copyPixels()
            {
                std::uint32_t pixelCount = sizeof(float) * (m_image.width * m_image.height);

                Buffer::CreateInfo createInfo {};
                createInfo.logicalDevice    = m_logicalDevice;
                createInfo.physicalDevice   = m_physicalDevice;
                createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
                createInfo.usage            = vk::BufferUsageFlagBits::eTransferSrc;
                createInfo.size             = pixelCount * Chicane::CubeMap::IMAGE_COUNT;

                Buffer::Instance stagingBuffer;
                Buffer::init(stagingBuffer, createInfo);

                for (std::uint32_t i = 0; i < Chicane::CubeMap::IMAGE_ORDER.size(); i++)
                {
                    const Chicane::Image::Data& image = m_images.at(Chicane::CubeMap::IMAGE_ORDER.at(i));

                    void* writeLocation = m_logicalDevice.mapMemory(
                        stagingBuffer.memory,
                        pixelCount * i,
                        pixelCount
                    );

                    memcpy(writeLocation, image.pixels, pixelCount);

                    m_logicalDevice.unmapMemory(stagingBuffer.memory);
                }

                Image::transitionLayout(
                    m_commandBuffer,
                    m_queue,
                    m_image.instance,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal,
                    Chicane::CubeMap::IMAGE_COUNT
                );

                Image::copyBufferToImage(
                    m_commandBuffer,
                    m_queue,
                    stagingBuffer.instance,
                    m_image.instance,
                    m_image.width,
                    m_image.height,
                    Chicane::CubeMap::IMAGE_COUNT
                );

                Image::transitionLayout(
                    m_commandBuffer,
                    m_queue,
                    m_image.instance,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    Chicane::CubeMap::IMAGE_COUNT
                );

                Buffer::destroy(m_logicalDevice, stagingBuffer);
            }

            void Instance::initDescriptorSet()
            {
                Descriptor::allocalteSet(
                    m_descriptor.set,
                    m_logicalDevice,
                    m_descriptor.setLayout,
                    m_descriptor.pool
                );

                vk::DescriptorImageInfo descriptorInfo {};
                descriptorInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                descriptorInfo.imageView   = m_image.view;
                descriptorInfo.sampler     = m_image.sampler;

                vk::WriteDescriptorSet descriptorSetInfo {};
                descriptorSetInfo.dstSet          = m_descriptor.set;
                descriptorSetInfo.dstBinding      = 0;
                descriptorSetInfo.dstArrayElement = 0;
                descriptorSetInfo.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
                descriptorSetInfo.descriptorCount = 1;
                descriptorSetInfo.pImageInfo      = &descriptorInfo;

                m_logicalDevice.updateDescriptorSets(
                    descriptorSetInfo,
                    nullptr
                );
            }
        }
    }
}