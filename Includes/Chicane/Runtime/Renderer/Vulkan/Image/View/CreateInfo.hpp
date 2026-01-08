#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace View
            {
                struct CHICANE_RUNTIME CreateInfo
                {
                public:
                    std::uint32_t        count;
                    vk::ImageViewType    type;
                    vk::ImageAspectFlags aspect;
                    vk::Format           format;  

                    // Devices
		    	    vk::Device           logicalDevice;
                };
            }
        }
    }
}