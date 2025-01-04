#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Image
        {
            struct CreateInfo
            {
            public:
                // Settings
                std::uint32_t           width;
		    	std::uint32_t           height;
                std::uint32_t           count;

                // Vulkan
		    	vk::Device              logicalDevice;
		    	vk::PhysicalDevice      physicalDevice;
                vk::ImageTiling         tiling;
                vk::ImageUsageFlags     usage;
                vk::ImageCreateFlags    create;
                vk::MemoryPropertyFlags memoryProperties;
                vk::Format              format;
            }; 
        }
    }
}