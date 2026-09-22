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

    CHICANE_GRID String toString(Grid::StyleImportType inValue);
}
