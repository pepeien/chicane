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

    inline Chicane::String toString(ExplorerFilter inValue)
    {
        switch (inValue)
        {
        case ExplorerFilter::All:
            return "All";

        case ExplorerFilter::Folders:
            return "Folders";

        case ExplorerFilter::Files:
            return "Files";

        default:
            return "";
        }
    }
}
