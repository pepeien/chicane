#pragma once

#include "Chicane/Box.hpp"

namespace Chicane
{
    namespace Box
    {
        enum class AssetType : std::uint8_t
        {
            Undefined, // N/A
            Animation, // Skeletal clip that references a `Skeleton`
            Font,      // Text Font
            Mesh,      // Two entries [`Model`, `Texture`] both being references
            Model,     // 3D Model
            Skeleton,  // Bone hierarchy referenced by `Mesh` and `Animation`
            Texture,   // Image
            Sky,       // `Texture`s compilation with a `Model` shape that produces a sky
            Sound,     // Sound
        };
    }
}