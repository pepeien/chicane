#pragma once

#include "Chicane/Box/Texture/Gltf/Entry.hpp"
#include "Chicane/Box/Texture/Gltf/Parsed.hpp"
#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace TextureGltf
        {
            CHICANE_BOX Parsed parse(const FileSystem::Path& inFilepath);
        }
    }
}
