#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_ENUM()
    enum class ExplorerItemKind : std::uint8_t
    {
        Folder,
        File,
        Preview
    };

    inline Chicane::String toString(ExplorerItemKind inValue)
    {
        switch (inValue)
        {
        case ExplorerItemKind::Folder:
            return "Folder";

        case ExplorerItemKind::File:
            return "File";

        case ExplorerItemKind::Preview:
            return "Preview";

        default:
            return "";
        }
    }
}
