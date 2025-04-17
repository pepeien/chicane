#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"
#include "Runtime/Renderer/Vulkan/Frame.hpp"
#include "Runtime/Renderer/Vulkan/Queue.hpp"
#include "Runtime/Renderer/Vulkan/Image.hpp"
#include "Runtime/Renderer/Vulkan/SwapChain/Bundle.hpp"
#include "Runtime/Renderer/Vulkan/SwapChain/SupportDetails.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            CHICANE void querySupport(
                SupportDetails& outSupportDetails,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            CHICANE void pickSurfaceFormat(
                vk::SurfaceFormatKHR& outSurfaceFormat,
                const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
            );
            CHICANE void pickPresentMode(
                vk::PresentModeKHR& outPresentMode,
                const std::vector<vk::PresentModeKHR>& inPresentModes
            );

            CHICANE void init(
                Bundle& outSwapChain,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        }
    }
}