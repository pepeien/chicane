#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"

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