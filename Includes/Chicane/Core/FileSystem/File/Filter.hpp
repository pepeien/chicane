#pragma once

#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE FileFilter
        {
        public:
            using List = std::vector<FileFilter>;

        public:
            String                        title      = "";
            std::vector<FileSystem::Path> extensions = {};
        };
    }
}