#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Memory
            {
                struct CreateInfo
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