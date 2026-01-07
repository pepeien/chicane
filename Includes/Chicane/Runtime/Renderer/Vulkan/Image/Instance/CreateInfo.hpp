#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Instance
            {
                struct CHICANE_RUNTIME CreateInfo
                {
                public:
                    std::uint32_t        width;
		    	    std::uint32_t        height;
                    std::uint32_t        count;

                    vk::ImageTiling      tiling;
                    vk::ImageCreateFlags flags;
                    vk::ImageUsageFlags  usage;
                    vk::Format           format;

		    	    vk::Device           logicalDevice;
                };
            }
        }
    }
}