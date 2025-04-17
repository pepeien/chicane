#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace SwapChain
        {
            struct CHICANE SupportDetails
            {
            public:
                vk::SurfaceCapabilitiesKHR        capabilities;
                std::vector<vk::SurfaceFormatKHR> formats;
                std::vector<vk::PresentModeKHR>   presentModes;
            };
        }
    }
}