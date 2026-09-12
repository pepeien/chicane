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

        inline String toString(ItemType inValue)
        {
            switch (inValue)
            {
            case ItemType::Undefined:
                return "Undefined";

            case ItemType::Folder:
                return "Folder";

            case ItemType::File:
                return "File";

            default:
                return "";
            }
        }
    }
}