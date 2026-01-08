#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        enum class ItemType : std::uint8_t
        {
            Undefined,
            Folder,
            File
        };
    }
}