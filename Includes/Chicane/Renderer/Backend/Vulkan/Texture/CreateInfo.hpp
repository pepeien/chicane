#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image.hpp"

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        struct CHICANE_RENDERER VulkanTextureCreateInfo
        {
        public:
            Image              image;

            vk::Device         logicalDevice;
            vk::PhysicalDevice physicalDevice;
            vk::CommandBuffer  commandBuffer;
            vk::Queue          queue;
        };
    }
}