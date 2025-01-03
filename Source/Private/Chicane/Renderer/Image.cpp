#include "Chicane/Renderer/Image.hpp"

#include "Chicane/Renderer/Device.hpp"
#include "Chicane/Renderer/CommandBuffer/Worker.hpp"

namespace Chicane
{
    namespace Image
    {
        vk::Format findSupportedFormat(
            const vk::PhysicalDevice& inPhysicalDevice,
            const std::vector<vk::Format>& inCandidates,
            const vk::ImageTiling& inTiling,
            const vk::FormatFeatureFlags& inFeatures
        )
        {
            for (vk::Format format : inCandidates)
            {
                vk::FormatProperties properties = inPhysicalDevice.getFormatProperties(format);

                bool bIsTilingLinear                 = inTiling == vk::ImageTiling::eLinear;
                bool bDoesFeatureSupportLinearTiling = (
                    properties.linearTilingFeatures & inFeatures
                ) == inFeatures;

                if (bIsTilingLinear && bDoesFeatureSupportLinearTiling)
                {
                    return format;
                }

                bool bIsTilingOptimal                 = inTiling == vk::ImageTiling::eOptimal;
                bool bDoesFeatureSupportOptimapTiling = (
                    properties.optimalTilingFeatures & inFeatures
                ) == inFeatures;

                if (bIsTilingOptimal && bDoesFeatureSupportOptimapTiling)
                {
                    return format;
                }
            }

            throw std::runtime_error("Unable to select suitable format");
        }

		void init(vk::Image& outImage, const CreateInfo& inCreateInfo)
        {
            vk::ImageCreateInfo createInfo;
            createInfo.flags         = vk::ImageCreateFlagBits() | inCreateInfo.create;
            createInfo.imageType     = vk::ImageType::e2D;
            createInfo.extent        = vk::Extent3D(inCreateInfo.width, inCreateInfo.height, 1);
            createInfo.mipLevels     = 1;
            createInfo.arrayLayers   = inCreateInfo.count;
            createInfo.format        = inCreateInfo.format;
            createInfo.samples       = vk::SampleCountFlagBits::e1;
            createInfo.tiling        = inCreateInfo.tiling;
            createInfo.initialLayout = vk::ImageLayout::eUndefined;
            createInfo.usage         = inCreateInfo.usage;
            createInfo.sharingMode   = vk::SharingMode::eExclusive;

            outImage = inCreateInfo.logicalDevice.createImage(createInfo);
        }

        void initMemory(
            vk::DeviceMemory& outDeviceMemory,
            const CreateInfo& inCreateInfo,
            const vk::Image& inImage
        )
        {
            vk::MemoryRequirements requirements = inCreateInfo
                .logicalDevice
                .getImageMemoryRequirements(inImage);

            vk::MemoryAllocateInfo allocationInfo;
            allocationInfo.allocationSize  = requirements.size;
            allocationInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                inCreateInfo.physicalDevice,
                requirements.memoryTypeBits,
                inCreateInfo.memoryProperties
            );

            outDeviceMemory = inCreateInfo.logicalDevice.allocateMemory(allocationInfo);
            inCreateInfo.logicalDevice.bindImageMemory(
                inImage,
                outDeviceMemory,
                0
            );
        }

        void initView(
            vk::ImageView& outImageView,
            const vk::Device& inLogicalDevice,
            const vk::Image& inImage,
            const vk::Format& inFormat,
            const vk::ImageAspectFlags& inAspect,
            vk::ImageViewType inViewType,
            std::uint32_t inCount
        )
        {
            vk::ImageViewCreateInfo createInfo {};
            createInfo.image                           = inImage;
            createInfo.viewType                        = inViewType;
            createInfo.format                          = inFormat;
            createInfo.components.r                    = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g                    = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b                    = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a                    = vk::ComponentSwizzle::eIdentity;
            createInfo.subresourceRange.aspectMask     = inAspect;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = inCount;

            outImageView = inLogicalDevice.createImageView(createInfo);
        }

        void transitionLayout(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Image& inImage,
            const vk::ImageLayout& inOldLayout,
            const vk::ImageLayout& inNewLayout,
            std::uint32_t inCount
        )
        {
            CommandBuffer::Worker::startJob(inCommandBuffer);

            vk::ImageMemoryBarrier imageMemoryBarrier;
            imageMemoryBarrier.oldLayout                       = inOldLayout;
            imageMemoryBarrier.newLayout                       = inNewLayout;
            imageMemoryBarrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            imageMemoryBarrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
            imageMemoryBarrier.image                           = inImage;
            imageMemoryBarrier.srcAccessMask                   = vk::AccessFlagBits::eTransferWrite;
            imageMemoryBarrier.dstAccessMask                   = vk::AccessFlagBits::eShaderRead;
            imageMemoryBarrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            imageMemoryBarrier.subresourceRange.baseMipLevel   = 0;
            imageMemoryBarrier.subresourceRange.levelCount     = 1;
            imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
            imageMemoryBarrier.subresourceRange.layerCount     = inCount;

            vk::PipelineStageFlags sourceStage      = vk::PipelineStageFlagBits::eTransfer;
            vk::PipelineStageFlags destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

            if (
                inOldLayout == vk::ImageLayout::eUndefined &&
                inNewLayout == vk::ImageLayout::eTransferDstOptimal
            )
            {
                imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
                imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eTransfer;
            }

            inCommandBuffer.pipelineBarrier(
                sourceStage,
                destinationStage,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                imageMemoryBarrier
            );

            CommandBuffer::Worker::endJob(inCommandBuffer, inQueue, "Transition Image Layout");
        }

        void copyBufferToImage(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Buffer& inSourceBuffer,
            const vk::Image& inDestinationImage,
            std::uint32_t inWidth,
            std::uint32_t inHeight,
            std::uint32_t inCount
        )
        {
            CommandBuffer::Worker::startJob(inCommandBuffer);

            vk::ImageSubresourceLayers imageSubresourceLayers;
            imageSubresourceLayers.aspectMask     = vk::ImageAspectFlagBits::eColor;
            imageSubresourceLayers.mipLevel       = 0;
            imageSubresourceLayers.baseArrayLayer = 0;
            imageSubresourceLayers.layerCount     = inCount;

            vk::BufferImageCopy bufferImageCopy;
            bufferImageCopy.bufferOffset      = 0;
            bufferImageCopy.bufferRowLength   = 0;
            bufferImageCopy.bufferImageHeight = 0;
            bufferImageCopy.imageSubresource  = imageSubresourceLayers;
            bufferImageCopy.imageOffset       = vk::Offset3D(0, 0, 0);
            bufferImageCopy.imageExtent       = vk::Extent3D(inWidth, inHeight, 1);

            inCommandBuffer.copyBufferToImage(
                inSourceBuffer,
                inDestinationImage,
                vk::ImageLayout::eTransferDstOptimal,
                bufferImageCopy
            );

            CommandBuffer::Worker::endJob(
                inCommandBuffer,
                inQueue,
                "Copy Buffer To Image"
            );
        }
	}
}