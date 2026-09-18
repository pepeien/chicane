#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Path.hpp"

namespace Chicane
{
    namespace Module
    {
        CHICANE_CORE bool load(const FileSystem::Path& inPath);
        CHICANE_CORE bool isLoaded(const FileSystem::Path& inPath);
        CHICANE_CORE void unloadAll();
    }
}
