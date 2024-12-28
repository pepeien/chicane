#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
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