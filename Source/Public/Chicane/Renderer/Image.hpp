#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Image/CreateInfo.hpp"

namespace Chicane
{
    namespace Image
    {
        vk::Format findSupportedFormat(
            const vk::PhysicalDevice& inPhysicalDevice,
            const std::vector<vk::Format>& inCandidates,
            const vk::ImageTiling& inTiling,
            const vk::FormatFeatureFlags& inFeatures
        );

		void init(vk::Image& outImage, const CreateInfo& inCreateInfo);
        void initMemory(
            vk::DeviceMemory& outDeviceMemory,
            const CreateInfo& inCreateInfo,
            const vk::Image& inImage
        );
        void initView(
            vk::ImageView& outImageView,
            const vk::Device& inLogicalDevice,
            const vk::Image& inImage,
            const vk::Format& inFormat,
            const vk::ImageAspectFlags& inAspect,
            vk::ImageViewType inViewType,
            std::uint32_t inCount
        );

        void transitionLayout(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Image& inImage,
            const vk::ImageLayout& inOldLayout,
            const vk::ImageLayout& inNewLayout,
            std::uint32_t inCount
        );
        void copyBufferToImage(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Buffer& inSourceBuffer,
            const vk::Image& inDestinationImage,
            std::uint32_t inWidth,
            std::uint32_t inHeight,
            std::uint32_t inCount
        );
	}
}