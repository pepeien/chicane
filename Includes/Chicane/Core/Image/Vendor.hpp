#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    enum class ImageVendor : std::uint8_t
    {
        Undefined, // N/A
        Jpg,       // .jpg | .jpeg
        Png,       // .png
        Gif,       // .gif
    };

    CHICANE_CORE String toString(ImageVendor inValue);
}