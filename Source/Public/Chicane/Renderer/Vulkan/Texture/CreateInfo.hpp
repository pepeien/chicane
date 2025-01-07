#pragma once

#include "Chicane/Core/Image.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            struct CreateInfo
            {
            public:
                Chicane::Image::Data image;

                vk::Device           logicalDevice;
                vk::PhysicalDevice   physicalDevice;
                vk::CommandBuffer    commandBuffer;
                vk::Queue            queue;
            };
        }
    }
}