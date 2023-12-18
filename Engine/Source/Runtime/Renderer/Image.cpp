#include "Image.hpp"

namespace Engine
{
    namespace Image
    {
		void init(vk::Image& outImage, const CreateInfo& inCreateInfo)
        {
            vk::ImageCreateInfo createInfo;
            createInfo.flags         = vk::ImageCreateFlags();
            createInfo.imageType     = vk::ImageType::e2D;
            createInfo.extent        = vk::Extent3D(inCreateInfo.width, inCreateInfo.height, 1);
            createInfo.mipLevels     = 1;
            createInfo.arrayLayers   = 1;
            createInfo.format        = vk::Format::eR8G8B8A8Unorm;
            createInfo.tiling        = inCreateInfo.tiling;
            createInfo.initialLayout = vk::ImageLayout::eUndefined;
            createInfo.usage         = inCreateInfo.usage;
            createInfo.sharingMode   = vk::SharingMode::eExclusive;
            createInfo.samples       = vk::SampleCountFlagBits::e1;

            outImage = inCreateInfo.logicalDevice.createImage(createInfo);
        }

        void initMemory(vk::DeviceMemory& outDeviceMemory, const CreateInfo& inCreateInfo, const vk::Image& inImage)
        {
            vk::MemoryRequirements requirements = inCreateInfo.logicalDevice.getImageMemoryRequirements(inImage);

            vk::MemoryAllocateInfo allocationInfo;
            allocationInfo.allocationSize = requirements.size;
            allocationInfo.memoryTypeIndex = Device::findMemoryTypeIndex(
                inCreateInfo.physicalDevice,
                requirements.memoryTypeBits,
                inCreateInfo.memoryProperties
            );

            outDeviceMemory = inCreateInfo.logicalDevice.allocateMemory(allocationInfo);
            inCreateInfo.logicalDevice.bindImageMemory(inImage, outDeviceMemory, 0);
        }

        void initView(
            vk::ImageView& outImageView,
            const vk::Device& inLogicalDevice,
            const vk::Image& inImage,
            const vk::Format& inFormat
        )
        {
            vk::ImageViewCreateInfo createInfo = {};
            createInfo.image    = inImage;
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format   = inFormat;
    
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    
            createInfo.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            outImageView = inLogicalDevice.createImageView(createInfo);
        }

        void transitionLayout(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Image& inImage,
            const vk::ImageLayout& oldLayout,
            const vk::ImageLayout& newLayout
        )
        {
            Command::Job::start(inCommandBuffer);

            vk::ImageSubresourceRange imageSubresourceRange;
            imageSubresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            imageSubresourceRange.baseMipLevel   = 0;
            imageSubresourceRange.levelCount     = 1;
            imageSubresourceRange.baseArrayLayer = 0;
            imageSubresourceRange.layerCount     = 1;

            vk::ImageMemoryBarrier imageMemoryBarrier;
            imageMemoryBarrier.oldLayout           = oldLayout;
            imageMemoryBarrier.newLayout           = newLayout;
            imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imageMemoryBarrier.image               = inImage;
            imageMemoryBarrier.subresourceRange    = imageSubresourceRange;

            vk::PipelineStageFlags sourceStage;
            vk::PipelineStageFlags destinationStage;

            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;

            if (oldLayout != vk::ImageLayout::eUndefined)
            {
                imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                sourceStage      = vk::PipelineStageFlagBits::eTransfer;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }

            inCommandBuffer.pipelineBarrier(
                sourceStage,
                destinationStage,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                imageMemoryBarrier
            );

            Command::Job::end(inCommandBuffer, inQueue, "Transition Image Layout");
        }

        void copyBufferToImage(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Buffer& inSourceBuffer,
            const vk::Image& inDestionationImage,
            const uint32_t& inWidth,
            const uint32_t& inHeight
        )
        {
            Command::Job::start(inCommandBuffer);

            vk::ImageSubresourceLayers imageSubresourceLayers;
            imageSubresourceLayers.aspectMask     = vk::ImageAspectFlagBits::eColor;
            imageSubresourceLayers.mipLevel       = 0;
            imageSubresourceLayers.baseArrayLayer = 0;
            imageSubresourceLayers.layerCount     = 1;

            vk::BufferImageCopy bufferImageCopy;
            bufferImageCopy.bufferOffset      = 0;
            bufferImageCopy.bufferRowLength   = 0;
            bufferImageCopy.bufferImageHeight = 0;
            bufferImageCopy.imageSubresource = imageSubresourceLayers;
            bufferImageCopy.imageOffset      = vk::Offset3D(0, 0, 0);
            bufferImageCopy.imageExtent      = vk::Extent3D(inWidth, inHeight, 1);

            inCommandBuffer.copyBufferToImage(
                inSourceBuffer,
                inDestionationImage,
                vk::ImageLayout::eTransferDstOptimal,
                bufferImageCopy
            );

            Command::Job::end(inCommandBuffer, inQueue, "Copy Buffer To Image");
        }
	}
}