#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Data.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Memory/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Sampler/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/View/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanImage
        {
            CHICANE_RENDERER vk::Format findSupportedFormat(
                const vk::PhysicalDevice&      inPhysicalDevice,
                const std::vector<vk::Format>& inCandidates,
                const vk::ImageTiling&         inTiling,
                const vk::FormatFeatureFlags&  inFeatures
            );

            CHICANE_RENDERER void initInstance(
                vk::Image& outInstance, const VulkanImageCreateInfo& inCreateInfo
            );
            CHICANE_RENDERER void initSampler(
                vk::Sampler& outSampler, const VulkanImageSamplerCreateInfo& inCreateInfo
            );
            CHICANE_RENDERER void initMemory(
                vk::DeviceMemory&                  outMemory,
                const vk::Image&                   inInstance,
                const VulkanImageMemoryCreateInfo& inCreateInfo
            );
            CHICANE_RENDERER void initView(
                vk::ImageView&                   outImageView,
                const vk::Image&                 inInstance,
                const VulkanImageViewCreateInfo& inCreateInfo
            );

            CHICANE_RENDERER void transitionLayout(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const vk::Image&         inImage,
                const vk::ImageLayout&   inOldLayout,
                const vk::ImageLayout&   inNewLayout,
                std::uint32_t            inCount
            );
            CHICANE_RENDERER void copyBufferToImage(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const vk::Buffer&        inSourceBuffer,
                const vk::Image&         inDestinationImage,
                std::uint32_t            inWidth,
                std::uint32_t            inHeight,
                std::uint32_t            inCount
            );
        }
    }
}