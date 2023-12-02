#pragma once

#include "Base.h"

namespace Engine
{
    namespace Core
    {
        namespace SwapChain
        {
            struct SupportDetails
            {
            public:
                vk::SurfaceCapabilitiesKHR capabilities;

                std::vector<vk::SurfaceFormatKHR> formats;
                std::vector<vk::PresentModeKHR> presentModes;
            };
        }
    }
}