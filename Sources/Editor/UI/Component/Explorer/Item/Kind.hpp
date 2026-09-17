#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>

namespace Editor
{
    CH_ENUM()
    enum class ExplorerItemKind : std::uint8_t
    {
        Folder,
        File,
        Preview
    };
}
