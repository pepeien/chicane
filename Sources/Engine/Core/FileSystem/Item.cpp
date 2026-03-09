#include "Chicane/Core/FileSystem/Item.hpp"

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
                childCount = static_cast<std::uint32_t>(ls(path.lexicallyNormal(), 1).size());

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
              childCount(0U)
        {}
    }
}