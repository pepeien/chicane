#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Image.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Queue.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/Bundle.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Swapchain/SupportDetails.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanSwapchain
        {
            CHICANE_RENDERER void querySupport(
                VulkanSwapchainSupportDetails& outSupportDetails,
                const vk::PhysicalDevice&      inPhysicalDevice,
                const vk::SurfaceKHR&          inSurface
            );

            CHICANE_RENDERER void pickSurfaceFormat(
                vk::SurfaceFormatKHR& outSurfaceFormat, const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
            );
            CHICANE_RENDERER void pickPresentMode(
                vk::PresentModeKHR& outPresentMode, const std::vector<vk::PresentModeKHR>& inPresentModes
            );

            CHICANE_RENDERER vk::Extent2D chooseExtent(
                const vk::SurfaceCapabilitiesKHR& inCapabilities, const vk::Extent2D& inFallback
            );

            CHICANE_RENDERER bool init(
                VulkanSwapchainBundle&    outSwapchain,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device&         inLogicalDevice,
                const vk::SurfaceKHR&     inSurface,
                const vk::SwapchainKHR&   inOldSwapchain,
                const vk::Extent2D&       inFallbackExtent
            );
        }
    }
}