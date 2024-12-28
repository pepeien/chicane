#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace SwapChain
    {
        struct SupportDetails
        {
            vk::SurfaceCapabilitiesKHR        capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR>   presentModes;
        };
    }
}