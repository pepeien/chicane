#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Texture.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanTextureCreateInfo
        {
        public:
            const DrawTexture* texture = nullptr;
            Image::Reference   image;

            std::uint32_t      residentMinMip = 0;

            vk::Device         logicalDevice;
            vk::PhysicalDevice physicalDevice;
            vk::CommandBuffer  commandBuffer;
            vk::Queue          queue;
        };
    }
}
