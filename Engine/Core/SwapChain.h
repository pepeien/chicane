#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace Engine
{
    namespace Core
    {
        struct SwapChainSupportDetails
        {
        public:
            VkSurfaceCapabilitiesKHR capabilities;

            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };
    }
}