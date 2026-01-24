#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {

        struct CHICANE_RENDERER VulkanImageViewCreateInfo
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