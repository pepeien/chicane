#pragma once

#include <cstdint>
#include <vector>

#include "Chicane/Core.hpp"
#include "Chicane/Core/FileSystem/Item/Type.hpp"
#include "Chicane/Core/FileSystem/Path.hpp"
#include "Chicane/Core/Reflection.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        CH_TYPE(Manual)
        struct CHICANE_CORE Item
        {
        public:
            using List = std::vector<Item>;

        public:
            Item(ItemType inType, const Path& inPath);
            Item();

        public:
            CH_FIELD()
            ItemType type;

            CH_FIELD()
            String name;

            CH_FIELD()
            Path path;

            CH_FIELD()
            String extension; // Only populated valid type is `ItemType::File`

            CH_FIELD()
            std::vector<Item> children; // Only populated valid type is `ItemType::Folder`
        };
    }
}