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
            std::uint32_t          mipLevels      = 1;
            bool                   bAnisotropy    = false;
            float                  maxAnisotropy  = 1.0f;

            vk::SamplerAddressMode addressMode;
            vk::BorderColor        borderColor;

            vk::Device             logicalDevice;
        };
    }
}