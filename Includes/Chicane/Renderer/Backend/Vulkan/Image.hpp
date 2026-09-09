#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Memory/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/Sampler/CreateInfo.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image/View/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanImage
        {
            inline bool hasStencil(vk::Format inFormat)
            {
                return inFormat == vk::Format::eD16UnormS8Uint || inFormat == vk::Format::eD24UnormS8Uint ||
                       inFormat == vk::Format::eD32SfloatS8Uint || inFormat == vk::Format::eS8Uint;
            }

            inline vk::ImageAspectFlags depthAspect(vk::Format inFormat)
            {
                vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth;
                if (hasStencil(inFormat))
                {
                    aspect |= vk::ImageAspectFlagBits::eStencil;
                }

                return aspect;
            }

            CHICANE_RENDERER vk::Format findSupportedFormat(
                const vk::PhysicalDevice&      inPhysicalDevice,
                const std::vector<vk::Format>& inCandidates,
                const vk::ImageTiling&         inTiling,
                const vk::FormatFeatureFlags&  inFeatures
            );

            CHICANE_RENDERER void initInstance(vk::Image& outInstance, const VulkanImageCreateInfo& inCreateInfo);
            CHICANE_RENDERER void initSampler(
                vk::Sampler& outSampler, const VulkanImageSamplerCreateInfo& inCreateInfo
            );
            CHICANE_RENDERER void initMemory(
                vk::DeviceMemory&                  outMemory,
                const vk::Image&                   inInstance,
                const VulkanImageMemoryCreateInfo& inCreateInfo
            );
            CHICANE_RENDERER void initView(
                vk::ImageView& outImageView, const vk::Image& inInstance, const VulkanImageViewCreateInfo& inCreateInfo
            );

            CHICANE_RENDERER void transitionLayout(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const vk::Image&         inImage,
                const vk::ImageLayout&   inOldLayout,
                const vk::ImageLayout&   inNewLayout,
                std::uint32_t            inCount,
                std::uint32_t            inLevelCount = 1
            );
            CHICANE_RENDERER void generateMipmaps(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue&         inQueue,
                const vk::Image&         inImage,
                std::uint32_t            inWidth,
                std::uint32_t            inHeight,
                std::uint32_t            inCount,
                std::uint32_t            inLevelCount
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