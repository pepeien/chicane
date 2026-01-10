#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            struct CHICANE_RUNTIME SupportDetails
            {
            public:
                vk::SurfaceCapabilitiesKHR        capabilities;
                std::vector<vk::SurfaceFormatKHR> formats;
                std::vector<vk::PresentModeKHR>   presentModes;
            };
        }
    }
}