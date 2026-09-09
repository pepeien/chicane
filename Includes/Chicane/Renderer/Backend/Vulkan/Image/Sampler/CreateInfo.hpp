#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageSamplerCreateInfo
        {
        public:
            std::uint32_t          mipLevels = 1;

            vk::SamplerAddressMode addressMode;
            vk::BorderColor        borderColor;

            vk::Device             logicalDevice;
        };
    }
}