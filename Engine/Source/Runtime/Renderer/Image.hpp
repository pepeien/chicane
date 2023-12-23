#pragma once

#include "Base.hpp"

#include "Device.hpp"
#include "Command/Worker.hpp"

namespace Chicane
{
    namespace Image
    {
        struct CreateInfo
        {
            uint32_t width;
			uint32_t height;
			std::string filename;
			vk::Device logicalDevice;
			vk::PhysicalDevice physicalDevice;
            vk::ImageTiling tiling;
            vk::ImageUsageFlags usage;
            vk::MemoryPropertyFlags memoryProperties;
            vk::Format format;
        }; 

		struct Bundle
		{
			vk::Image instance;
			vk::DeviceMemory memory;
			vk::ImageView view;
			vk::Sampler sampler;
		};

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
            const vk::ImageAspectFlags& inAspect
        );

        void transitionLayout(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Image& inImage,
            const vk::ImageLayout& inOldLayout,
            const vk::ImageLayout& inNewLayout
        );
        void copyBufferToImage(
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::Buffer& inSourceBuffer,
            const vk::Image& inDestinationImage,
            const uint32_t& inWidth,
            const uint32_t& inHeight
        );
	}
}