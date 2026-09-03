#pragma once

#include <cstdint>

namespace Editor
{
    enum class ExplorerFilter : std::uint8_t
    {
        All,
        Folders,
        Files
    };
}
