#include "Chicane/Runtime/Renderer/Vulkan/Image.hpp"

#include "Chicane/Runtime/Renderer/Vulkan/CommandBuffer/Worker.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Device.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            vk::Format findSupportedFormat(
                const vk::PhysicalDevice&      inPhysicalDevice,
                const std::vector<vk::Format>& inCandidates,
                const vk::ImageTiling&         inTiling,
                const vk::FormatFeatureFlags&  inFeatures
            )
            {
                for (vk::Format format : inCandidates)
                {
                    vk::FormatProperties properties =
                        inPhysicalDevice.getFormatProperties(format);

                    bool bIsTilingLinear = inTiling == vk::ImageTiling::eLinear;
                    bool bDoesFeatureSupportLinearTiling =
                        (properties.linearTilingFeatures & inFeatures) ==
                        inFeatures;

                    if (bIsTilingLinear && bDoesFeatureSupportLinearTiling)
                    {
                        return format;
                    }

                    bool bIsTilingOptimal =
                        inTiling == vk::ImageTiling::eOptimal;
                    bool bDoesFeatureSupportOptimapTiling =
                        (properties.optimalTilingFeatures & inFeatures) ==
                        inFeatures;

                    if (bIsTilingOptimal && bDoesFeatureSupportOptimapTiling)
                    {
                        return format;
                    }
                }

                throw std::runtime_error("Unable to select suitable format");
            }

            void initInstance(
                vk::Image& outInstance, const Instance::CreateInfo& inCreateInfo
            )
            {
                vk::ImageCreateInfo createInfo = {};
                createInfo.flags =
                    vk::ImageCreateFlagBits() | inCreateInfo.flags;
                createInfo.imageType = vk::ImageType::e2D;
                createInfo.extent =
                    vk::Extent3D(inCreateInfo.width, inCreateInfo.height, 1);
                createInfo.mipLevels     = 1;
                createInfo.arrayLayers   = inCreateInfo.count;
                createInfo.format        = inCreateInfo.format;
                createInfo.samples       = vk::SampleCountFlagBits::e1;
                createInfo.tiling        = inCreateInfo.tiling;
                createInfo.initialLayout = vk::ImageLayout::eUndefined;
                createInfo.usage         = inCreateInfo.usage;
                createInfo.sharingMode   = vk::SharingMode::eExclusive;

                outInstance =
                    inCreateInfo.logicalDevice.createImage(createInfo);
            }

            void initSampler(
                vk::Sampler& outSampler, const Sampler::CreateInfo& inCreateInfo
            )
            {
                vk::SamplerCreateInfo createInfo   = {};
                createInfo.flags                   = vk::SamplerCreateFlags();
                createInfo.minFilter               = vk::Filter::eNearest;
                createInfo.magFilter               = vk::Filter::eLinear;
                createInfo.addressModeU            = inCreateInfo.addressMode;
                createInfo.addressModeV            = inCreateInfo.addressMode;
                createInfo.addressModeW            = inCreateInfo.addressMode;
                createInfo.anisotropyEnable        = false;
                createInfo.maxAnisotropy           = 1.0f;
                createInfo.borderColor             = inCreateInfo.borderColor;
                createInfo.unnormalizedCoordinates = false;
                createInfo.compareEnable           = false;
                createInfo.compareOp               = vk::CompareOp::eAlways;
                createInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
                createInfo.mipLodBias = 0.0f;
                createInfo.minLod     = 0.0f;
                createInfo.maxLod     = 1.0f;

                outSampler =
                    inCreateInfo.logicalDevice.createSampler(createInfo);
            }

            void initMemory(
                vk::DeviceMemory&         outMemory,
                const vk::Image&          inInstance,
                const Memory::CreateInfo& inCreateInfo
            )
            {
                vk::MemoryRequirements requirements =
                    inCreateInfo.logicalDevice.getImageMemoryRequirements(
                        inInstance
                    );

                vk::MemoryAllocateInfo allocationInfo;
                allocationInfo.allocationSize  = requirements.size;
                allocationInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                    inCreateInfo.physicalDevice,
                    requirements.memoryTypeBits,
                    inCreateInfo.properties
                );

                outMemory =
                    inCreateInfo.logicalDevice.allocateMemory(allocationInfo);

                inCreateInfo.logicalDevice.bindImageMemory(
                    inInstance, outMemory, 0
                );
            }

            void initView(
                vk::ImageView&          outView,
                const vk::Image&        inInstance,
                const View::CreateInfo& inCreateInfo
            )
            {
                vk::ImageViewCreateInfo createInfo = {};
                createInfo.image                   = inInstance;
                createInfo.viewType                = inCreateInfo.type;
                createInfo.format                  = inCreateInfo.format;
                createInfo.components.r            = vk::ComponentSwizzle::eR;
                createInfo.components.g            = vk::ComponentSwizzle::eG;
                createInfo.components.b            = vk::ComponentSwizzle::eB;
                createInfo.components.a            = vk::ComponentSwizzle::eA;
                createInfo.subresourceRange.aspectMask   = inCreateInfo.aspect;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount   = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount     = inCreateInfo.count;

                outView =
                    inCreateInfo.logicalDevice.createImageView(createInfo);
            }

            void transitionLayout(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const vk::Image&         inImage,
                const vk::ImageLayout&   inOldLayout,
                const vk::ImageLayout&   inNewLayout,
                std::uint32_t            inCount
            )
            {
                CommandBuffer::Worker::startJob(inCommandBuffer);

                vk::ImageMemoryBarrier barrier = {};
                barrier.oldLayout              = inOldLayout;
                barrier.newLayout              = inNewLayout;
                barrier.srcQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex    = VK_QUEUE_FAMILY_IGNORED;
                barrier.image                  = inImage;
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
                barrier.subresourceRange.aspectMask =
                    vk::ImageAspectFlagBits::eColor;
                barrier.subresourceRange.baseMipLevel   = 0;
                barrier.subresourceRange.levelCount     = 1;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount     = inCount;

                vk::PipelineStageFlags sourceStage =
                    vk::PipelineStageFlagBits::eTransfer;
                vk::PipelineStageFlags destinationStage =
                    vk::PipelineStageFlagBits::eFragmentShader;

                if (inOldLayout == vk::ImageLayout::eUndefined &&
                    inNewLayout == vk::ImageLayout::eTransferDstOptimal)
                {
                    barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
                    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                    sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
                    destinationStage = vk::PipelineStageFlagBits::eTransfer;
                }

                inCommandBuffer.pipelineBarrier(
                    sourceStage,
                    destinationStage,
                    vk::DependencyFlags(),
                    nullptr,
                    nullptr,
                    barrier
                );

                CommandBuffer::Worker::endJob(
                    inCommandBuffer, inQueue, "Transition Image Layout"
                );
            }

            void copyBufferToImage(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const vk::Buffer&        inSourceBuffer,
                const vk::Image&         inDestinationImage,
                std::uint32_t            inWidth,
                std::uint32_t            inHeight,
                std::uint32_t            inCount
            )
            {
                CommandBuffer::Worker::startJob(inCommandBuffer);

                vk::ImageSubresourceLayers imageSubresourceLayers = {};
                imageSubresourceLayers.aspectMask =
                    vk::ImageAspectFlagBits::eColor;
                imageSubresourceLayers.mipLevel       = 0;
                imageSubresourceLayers.baseArrayLayer = 0;
                imageSubresourceLayers.layerCount     = inCount;

                vk::BufferImageCopy bufferImageCopy = {};
                bufferImageCopy.bufferOffset        = 0;
                bufferImageCopy.bufferRowLength     = 0;
                bufferImageCopy.bufferImageHeight   = 0;
                bufferImageCopy.imageSubresource    = imageSubresourceLayers;
                bufferImageCopy.imageOffset         = vk::Offset3D(0, 0, 0);
                bufferImageCopy.imageExtent =
                    vk::Extent3D(inWidth, inHeight, 1);

                inCommandBuffer.copyBufferToImage(
                    inSourceBuffer,
                    inDestinationImage,
                    vk::ImageLayout::eTransferDstOptimal,
                    bufferImageCopy
                );

                CommandBuffer::Worker::endJob(
                    inCommandBuffer, inQueue, "Copy Buffer To Image"
                );
            }
        }
    }
}