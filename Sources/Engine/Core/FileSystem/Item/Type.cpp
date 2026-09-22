#include "Chicane/Core/FileSystem/Item/Type.hpp"
#include "Chicane/Core/FileSystem/Item/Type.reflected.hpp"

namespace Chicane
{
    String toString(FileSystem::ItemType inValue)
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
