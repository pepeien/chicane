#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Core/Image.hpp"
#include "Chicane/Runtime.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            struct CHICANE_RUNTIME CreateInfo
            {
            public:
                Chicane::Image     image;

                vk::Device         logicalDevice;
                vk::PhysicalDevice physicalDevice;
                vk::CommandBuffer  commandBuffer;
                vk::Queue          queue;
            };
        }
    }
}