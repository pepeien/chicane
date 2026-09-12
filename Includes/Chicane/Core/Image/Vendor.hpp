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

    inline String toString(ImageVendor inValue)
    {
        switch (inValue)
        {
        case ImageVendor::Undefined:
            return "Undefined";

        case ImageVendor::Jpg:
            return "Jpg";

        case ImageVendor::Png:
            return "Png";

        case ImageVendor::Gif:
            return "Gif";

        default:
            return "";
        }
    }
}