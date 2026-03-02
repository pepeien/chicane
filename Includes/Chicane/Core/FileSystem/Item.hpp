#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/FileSystem/Item/Type.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        struct CHICANE_CORE Item
        {
        public:
            using List = std::vector<Item>;

        public:
            Item(ItemType inType, const Path& inPath);
            Item();

        public:
            ItemType      type;
            String        name;
            Path          path;

            // File only
            String        extension;

            // Folder only
            std::uint32_t childCount;
        };
    }
}