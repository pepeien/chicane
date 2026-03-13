#include "Chicane/Core/FileSystem/Item.reflected.hpp"

#include "Chicane/Core/FileSystem.hpp"

namespace Chicane
{
    namespace FileSystem
    {
        Item::Item(ItemType inType, const Path& inPath)
            : Item()
        {
            type = inType;
            path = inPath;
            name = path.filename().toString();

            switch (type)
            {
            case ItemType::File:
                extension = path.extension().toString();

                break;

            case ItemType::Folder:
                children = ls(path.lexicallyNormal(), 1);

                break;

            default:
                break;
            }
        }

        Item::Item()
            : type(ItemType::Undefined),
              name(""),
              path(""),
              extension(""),
              children({})
        {}
    }
}