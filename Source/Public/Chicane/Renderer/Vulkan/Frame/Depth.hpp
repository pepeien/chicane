#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            struct Depth
            {
                vk::Image        image;
                vk::ImageView    imageView;
                vk::DeviceMemory memory;
                vk::Format       format;
            };
        }
    }
}