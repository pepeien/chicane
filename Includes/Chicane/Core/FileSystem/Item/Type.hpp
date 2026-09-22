#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        CH_ENUM()
        enum class ItemType : std::uint8_t
        {
            Undefined,
            Folder,
            File
        };
    }

    CHICANE_CORE String toString(FileSystem::ItemType inValue);
}
