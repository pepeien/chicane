#pragma once

#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Route
        {
            String           path = {};
            FileSystem::Path file = {};
        };
    }
}
