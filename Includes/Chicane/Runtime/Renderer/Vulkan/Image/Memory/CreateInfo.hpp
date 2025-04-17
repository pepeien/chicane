#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            namespace Memory
            {
                struct CHICANE CreateInfo
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