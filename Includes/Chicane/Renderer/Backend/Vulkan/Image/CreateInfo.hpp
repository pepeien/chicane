#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageCreateInfo
        {
        public:
            std::uint32_t        width;
            std::uint32_t        height;
            std::uint32_t        count;
            std::uint32_t        mipLevels = 1;

            vk::ImageTiling      tiling;
            vk::ImageCreateFlags flags;
            vk::ImageUsageFlags  usage;
            vk::Format           format;

            vk::Device           logicalDevice;
        };
    }
}