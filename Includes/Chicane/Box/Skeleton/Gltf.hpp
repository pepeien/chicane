#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Skeleton/Bone.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        namespace SkeletonGltf
        {
            CHICANE_BOX SkeletonBone::List parse(const FileSystem::Path& inFilepath);
        }
    }
}
