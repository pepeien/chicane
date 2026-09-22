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

    Chicane::String toString(ExplorerItemKind inValue);
}
