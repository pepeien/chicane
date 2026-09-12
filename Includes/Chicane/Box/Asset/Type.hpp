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

        inline String toString(AssetType inValue)
        {
            switch (inValue)
            {
            case AssetType::Undefined:
                return "Undefined";

            case AssetType::Animation:
                return "Animation";

            case AssetType::Font:
                return "Font";

            case AssetType::Mesh:
                return "Mesh";

            case AssetType::Model:
                return "Model";

            case AssetType::Skeleton:
                return "Skeleton";

            case AssetType::Texture:
                return "Texture";

            case AssetType::Sky:
                return "Sky";

            case AssetType::Sound:
                return "Sound";

            default:
                return "";
            }
        }
    }
}