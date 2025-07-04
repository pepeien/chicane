#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image/Instance.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image/Memory.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image/Sampler.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image/View.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            CHICANE_RUNTIME vk::Format findSupportedFormat(
                const vk::PhysicalDevice& inPhysicalDevice,
                const std::vector<vk::Format>& inCandidates,
                const vk::ImageTiling& inTiling,
                const vk::FormatFeatureFlags& inFeatures
            );

	    	CHICANE_RUNTIME void initInstance(vk::Image& outInstance, const Instance::CreateInfo& inCreateInfo);
            CHICANE_RUNTIME void initSampler(vk::Sampler& outSampler, const Sampler::CreateInfo& inCreateInfo);
            CHICANE_RUNTIME void initMemory(vk::DeviceMemory& outMemory, const vk::Image& inInstance, const Memory::CreateInfo& inCreateInfo);
            CHICANE_RUNTIME void initView(vk::ImageView& outImageView, const vk::Image& inInstance, const View::CreateInfo& inCreateInfo);

            CHICANE_RUNTIME void transitionLayout(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue& inQueue,
                const vk::Image& inImage,
                const vk::ImageLayout& inOldLayout,
                const vk::ImageLayout& inNewLayout,
                std::uint32_t inCount
            );
            CHICANE_RUNTIME void copyBufferToImage(
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