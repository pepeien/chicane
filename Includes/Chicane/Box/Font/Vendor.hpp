#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class FontVendor : std::uint8_t
        {
            Undefined, // N/A
            OpenType,  // .otf
            TrueType   // .ttf
        };
    }
}