#pragma once

#include <vector>

#include "Chicane/Box.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace Box
    {
        struct CHICANE_BOX ImportResult
        {
        public:
            FileSystem::Path              primary;
            std::vector<FileSystem::Path> written;
        };
    }
}
