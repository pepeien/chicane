#pragma once

#include "Chicane/Box.hpp"
#include "Chicane/Box/Import/Result.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Box
    {
        CHICANE_BOX ImportResult importSource(
            const FileSystem::Path& inSource, const FileSystem::Path& inDirectory = {}, const String& inId = {}
        );
    }
}
