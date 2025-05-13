#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image/Data.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image/Instance.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image/Memory.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image/Sampler.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image/View.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            CHICANE vk::Format findSupportedFormat(
                const vk::PhysicalDevice& inPhysicalDevice,
                const std::vector<vk::Format>& inCandidates,
                const vk::ImageTiling& inTiling,
                const vk::FormatFeatureFlags& inFeatures
            );

	    	CHICANE void initInstance(vk::Image& outInstance, const Instance::CreateInfo& inCreateInfo);
            CHICANE void initSampler(vk::Sampler& outSampler, const Sampler::CreateInfo& inCreateInfo);
            CHICANE void initMemory(vk::DeviceMemory& outMemory, const vk::Image& inInstance, const Memory::CreateInfo& inCreateInfo);
            CHICANE void initView(vk::ImageView& outImageView, const vk::Image& inInstance, const View::CreateInfo& inCreateInfo);

            CHICANE void transitionLayout(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue& inQueue,
                const vk::Image& inImage,
                const vk::ImageLayout& inOldLayout,
                const vk::ImageLayout& inNewLayout,
                std::uint32_t inCount
            );
            CHICANE void copyBufferToImage(
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
}