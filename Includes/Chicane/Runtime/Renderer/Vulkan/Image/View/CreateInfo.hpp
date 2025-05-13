#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace View
            {
                struct CHICANE CreateInfo
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