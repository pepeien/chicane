#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanImageSamplerCreateInfo
        {
        public:
            vk::SamplerAddressMode addressMode;
            vk::BorderColor        borderColor;

            vk::Device             logicalDevice;
        };
    }
}