#pragma once

#include "Chicane/Box/Essential.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class AssetType : std::uint8_t
        {
            Undefined, // N/A
            Sound,     // Audio
            Font,      // Text Font
            Mesh,      // Two entries [Model, Texture] both being refNames
            Model,     // 3D Model
            Sky,       // 6 entries all being images [Positive X, Negative X, Positive Y, Negative Y, Positive Z, Negative Z]
            Texture,   // Image
        };
    }
}