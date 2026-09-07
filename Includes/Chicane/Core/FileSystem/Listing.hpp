#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Item.hpp"
#include "Chicane/Core/FileSystem/Path.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE Listing
        {
        public:
            Path       path;
            Item::List children;
        };
    }
}
