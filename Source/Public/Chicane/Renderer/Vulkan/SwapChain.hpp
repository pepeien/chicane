#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Frame.hpp"
#include "Chicane/Renderer/Vulkan/Queue.hpp"
#include "Chicane/Renderer/Vulkan/Image.hpp"
#include "Chicane/Renderer/Vulkan/SwapChain/Bundle.hpp"
#include "Chicane/Renderer/Vulkan/SwapChain/SupportDetails.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            void querySupport(
                SupportDetails& outSupportDetails,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            void pickSurfaceFormat(
                vk::SurfaceFormatKHR& outSurfaceFormat,
                const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
            );
            void pickPresentMode(
                vk::PresentModeKHR& outPresentMode,
                const std::vector<vk::PresentModeKHR>& inPresentModes
            );
            void pickExtent(
                vk::Extent2D& outExtent,
                std::uint32_t inWidth,
                std::uint32_t inHeight,
                const vk::SurfaceCapabilitiesKHR& inCapabilities
            );

            void init(
                Bundle& outSwapChain,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface,
                const Vec<2, int>& inResolution
            );
        }
    }
}