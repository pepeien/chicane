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
            std::uint32_t        mipLevels = 1;
            vk::ImageViewType    type;
            vk::ImageAspectFlags aspect;
            vk::Format           format;

            // Devices
            vk::Device           logicalDevice;
        };
    }
}