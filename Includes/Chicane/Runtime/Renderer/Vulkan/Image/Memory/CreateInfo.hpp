#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Memory
            {
                struct CHICANE_RUNTIME CreateInfo
                {
                public:
                    vk::MemoryPropertyFlags properties;

                    // Devices
		    	    vk::Device           logicalDevice;
		    	    vk::PhysicalDevice   physicalDevice;
                };
            }
        }
    }
}