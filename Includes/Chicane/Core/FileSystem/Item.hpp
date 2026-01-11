#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Item/Type.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE Item
        {
        public:
            ItemType      type       = ItemType::Undefined;
            String        name       = "";
            String        path       = "";

            // File only
            String        extension  = "";

            // Folder only
            std::uint32_t childCount = 0;
        };
    }
}