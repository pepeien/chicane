#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Frame.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Queue.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Image.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/SwapChain/Bundle.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/SwapChain/SupportDetails.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            CHICANE_RUNTIME void querySupport(
                SupportDetails& outSupportDetails,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        
            CHICANE_RUNTIME void pickSurfaceFormat(
                vk::SurfaceFormatKHR& outSurfaceFormat,
                const std::vector<vk::SurfaceFormatKHR>& inSurfaceFormats
            );
            CHICANE_RUNTIME void pickPresentMode(
                vk::PresentModeKHR& outPresentMode,
                const std::vector<vk::PresentModeKHR>& inPresentModes
            );

            CHICANE_RUNTIME void init(
                Bundle& outSwapChain,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Device& inLogicalDevice,
                const vk::SurfaceKHR& inSurface
            );
        }
    }
}