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

    inline String toString(FileSystem::ItemType inValue)
    {
        switch (inValue)
        {
        case FileSystem::ItemType::Undefined:
            return "Undefined";

        case FileSystem::ItemType::Folder:
            return "Folder";

        case FileSystem::ItemType::File:
            return "File";

        default:
            return "";
        }
    }
}
