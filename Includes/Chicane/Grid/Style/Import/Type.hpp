#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleImportType : std::uint8_t
        {
            Undefined,
            Style,
            Font,
            Texture
        };
    }

    inline String toString(Grid::StyleImportType inValue)
    {
        switch (inValue)
        {
        case Grid::StyleImportType::Undefined:
            return "Undefined";

        case Grid::StyleImportType::Style:
            return "Style";

        case Grid::StyleImportType::Font:
            return "Font";

        case Grid::StyleImportType::Texture:
            return "Texture";

        default:
            return "";
        }
    }
}
