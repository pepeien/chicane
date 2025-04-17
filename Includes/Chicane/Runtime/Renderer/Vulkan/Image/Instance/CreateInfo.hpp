#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Instance
            {
                struct CHICANE CreateInfo
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