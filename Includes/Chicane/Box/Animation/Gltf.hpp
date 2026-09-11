#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Box.hpp"
#include "Chicane/Box/Animation/Clip.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace AnimationGltf
        {
            CHICANE_BOX std::vector<String> list(const FileSystem::Path& inFilepath);
            CHICANE_BOX AnimationClip parse(const FileSystem::Path& inFilepath, const String& inId);
            CHICANE_BOX AnimationClip parse(const FileSystem::Path& inFilepath, std::uint32_t inIndex);
        }
    }
}
