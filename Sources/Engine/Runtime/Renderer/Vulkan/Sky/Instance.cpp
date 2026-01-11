#include "Chicane/Runtime/Renderer/Vulkan/Sky/Instance.hpp"

#include "Chicane/Box/Asset/Sky.hpp"
#include "Chicane/Box/Asset/Sky/Side.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Sky
        {
            Instance::Instance(const CreateInfo& inCreateInfo)
                : m_image(),
                  m_images(inCreateInfo.images),
                  m_logicalDevice(inCreateInfo.logicalDevice),
                  m_physicalDevice(inCreateInfo.physicalDevice),
                  m_commandBuffer(inCreateInfo.commandBuffer),
                  m_queue(inCreateInfo.queue),
                  m_descriptor({})
            {
                if (m_images.size() < Box::Sky::ORDER.size())
                {
                    throw std::runtime_error(
                        "Every cube map must have " + std::to_string(Box::Sky::ORDER.size()) + " images."
                    );
                }

                m_descriptor.setLayout = inCreateInfo.descriptorSetLayout;
                m_descriptor.set       = nullptr;
                m_descriptor.pool      = inCreateInfo.descriptorPool;

                m_image = Image::Data(m_images.at(Box::SkySide::Up));

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

            void Instance::bind(const vk::CommandBuffer& inCommandBuffer, const vk::PipelineLayout& inPipelineLayout)
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
                Image::Instance::CreateInfo instanceCreateInfo = {};
                instanceCreateInfo.width                       = m_image.getWidth();
                instanceCreateInfo.height                      = m_image.getHeight();
                instanceCreateInfo.count                       = static_cast<std::uint32_t>(Box::Sky::ORDER.size());
                instanceCreateInfo.tiling                      = vk::ImageTiling::eOptimal;
                instanceCreateInfo.flags                       = vk::ImageCreateFlagBits::eCubeCompatible;
                instanceCreateInfo.usage  = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
                instanceCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
                instanceCreateInfo.logicalDevice = m_logicalDevice;
                Image::initInstance(m_image.instance, instanceCreateInfo);

                Image::Sampler::CreateInfo samplerCreateInfo = {};
                samplerCreateInfo.addressMode                = vk::SamplerAddressMode::eRepeat;
                samplerCreateInfo.borderColor                = vk::BorderColor::eIntTransparentBlack;
                samplerCreateInfo.logicalDevice              = m_logicalDevice;
                Image::initSampler(m_image.sampler, samplerCreateInfo);

                Image::Memory::CreateInfo memoryCreateInfo = {};
                memoryCreateInfo.properties                = vk::MemoryPropertyFlagBits::eDeviceLocal;
                memoryCreateInfo.logicalDevice             = m_logicalDevice;
                memoryCreateInfo.physicalDevice            = m_physicalDevice;
                Image::initMemory(m_image.memory, m_image.instance, memoryCreateInfo);

                Image::View::CreateInfo viewCreateInfo = {};
                viewCreateInfo.count                   = instanceCreateInfo.count;
                viewCreateInfo.type                    = vk::ImageViewType::eCube;
                viewCreateInfo.aspect                  = vk::ImageAspectFlagBits::eColor;
                viewCreateInfo.format                  = instanceCreateInfo.format;
                viewCreateInfo.logicalDevice           = m_logicalDevice;
                Image::initView(m_image.view, m_image.instance, viewCreateInfo);
            }

            void Instance::copyPixels()
            {
                std::uint32_t pixelCount = sizeof(float) * m_image.getSize();

                BufferCreateInfo createInfo = {};
                createInfo.logicalDevice    = m_logicalDevice;
                createInfo.physicalDevice   = m_physicalDevice;
                createInfo.memoryProperties =
                    vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible;
                createInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
                createInfo.size  = pixelCount * Box::Sky::ORDER.size();

                Buffer stagingBuffer;
                stagingBuffer.init(createInfo);

                for (std::uint32_t i = 0; i < Box::Sky::ORDER.size(); i++)
                {
                    const auto& image = m_images.at(Box::Sky::ORDER.at(i));

                    void* writeLocation = m_logicalDevice.mapMemory(stagingBuffer.memory, pixelCount * i, pixelCount);

                    memcpy(writeLocation, image.getPixels(), pixelCount);

                    m_logicalDevice.unmapMemory(stagingBuffer.memory);
                }

                Image::transitionLayout(
                    m_commandBuffer,
                    m_queue,
                    m_image.instance,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal,
                    static_cast<std::uint32_t>(Box::Sky::ORDER.size())
                );

                Image::copyBufferToImage(
                    m_commandBuffer,
                    m_queue,
                    stagingBuffer.instance,
                    m_image.instance,
                    m_image.getWidth(),
                    m_image.getHeight(),
                    static_cast<std::uint32_t>(Box::Sky::ORDER.size())
                );

                Image::transitionLayout(
                    m_commandBuffer,
                    m_queue,
                    m_image.instance,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    static_cast<std::uint32_t>(Box::Sky::ORDER.size())
                );

                stagingBuffer.destroy(m_logicalDevice);
            }

            void Instance::initDescriptorSet()
            {
                Descriptor::allocalteSet(m_descriptor.set, m_logicalDevice, m_descriptor.setLayout, m_descriptor.pool);

                vk::DescriptorImageInfo descriptorInfo = {};
                descriptorInfo.imageLayout             = vk::ImageLayout::eShaderReadOnlyOptimal;
                descriptorInfo.imageView               = m_image.view;
                descriptorInfo.sampler                 = m_image.sampler;

                vk::WriteDescriptorSet descriptorSetInfo = {};
                descriptorSetInfo.dstSet                 = m_descriptor.set;
                descriptorSetInfo.dstBinding             = 0;
                descriptorSetInfo.dstArrayElement        = 0;
                descriptorSetInfo.descriptorType         = vk::DescriptorType::eCombinedImageSampler;
                descriptorSetInfo.descriptorCount        = 1;
                descriptorSetInfo.pImageInfo             = &descriptorInfo;

                m_logicalDevice.updateDescriptorSets(descriptorSetInfo, nullptr);
            }
        }
    }
}