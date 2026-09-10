#pragma once

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
            CHICANE_BOX AnimationClip parse(const FileSystem::Path& inFilepath, const String& inId);
        }
    }
}
