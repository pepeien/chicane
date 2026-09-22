#pragma once

#include <vector>

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
            Sound,     // Sound tracks
            Effect,    // Visual effects
        };

        CHICANE_BOX bool isFileAsset(const FileSystem::Path& inFilepath);

        CHICANE_BOX AssetType getTypeFromExtension(const FileSystem::Path& inFilepath);
        CHICANE_BOX AssetType getTypeFromTag(const String& inValue);

        CHICANE_BOX std::vector<String> getTypeTags();
        CHICANE_BOX const String& getTypeTag(AssetType inValue);

        CHICANE_BOX std::vector<String> getTypeExtensions();
        CHICANE_BOX const String& getTypeExtension(AssetType inValue);
    }

    CHICANE_BOX String toString(Box::AssetType inValue);
}
