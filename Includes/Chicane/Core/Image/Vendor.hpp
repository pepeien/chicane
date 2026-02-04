#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    enum class ImageVendor : std::uint8_t
    {
        Undefined, // N/A
        Jpg,       // .jpg | .jpeg
        Png,       // .png
    };
}