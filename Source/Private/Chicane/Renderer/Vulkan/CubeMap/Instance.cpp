#include "Chicane/Renderer/Vulkan/CubeMap/Instance.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Renderer/Vulkan/CubeMap/Instance.hpp"
#include "Chicane/Renderer/Vulkan/Image.hpp"
#include "Chicane/Renderer/Vulkan/Image/CreateInfo.hpp"

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

            void Instance::initImage()
            {
                Image::CreateInfo createInfo {};
                createInfo.width            = m_image.width;
                createInfo.height           = m_image.height;
                createInfo.count            = Chicane::CubeMap::IMAGE_COUNT;
                createInfo.logicalDevice    = m_logicalDevice;
                createInfo.physicalDevice   = m_physicalDevice;
                createInfo.tiling           = vk::ImageTiling::eOptimal;
                createInfo.usage            = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
                createInfo.create           = vk::ImageCreateFlagBits::eCubeCompatible;
                createInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
                createInfo.format           = vk::Format::eR8G8B8A8Unorm;

                Image::init(m_image.instance, createInfo);
                Image::initMemory(
                    m_image.memory,
                    createInfo,
                    m_image.instance
                );
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

            void Instance::initView()
            {
                Image::initView(
                    m_image.view,
                    m_logicalDevice,
                    m_image.instance,
                    vk::Format::eR8G8B8A8Unorm,
                    vk::ImageAspectFlagBits::eColor,
                    vk::ImageViewType::eCube,
                    Chicane::CubeMap::IMAGE_COUNT
                );
            }

            void Instance::initSampler()
            {
                vk::SamplerCreateInfo createInfo {};
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