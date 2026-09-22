#pragma once

#include <cstdint>

#include <Chicane/Core/String.hpp>

namespace Editor
{
    enum class ExplorerFilter : std::uint8_t
    {
        All,
        Folders,
        Files
    };

    Chicane::String toString(ExplorerFilter inValue);
}
