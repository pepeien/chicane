#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleLocationType : std::uint8_t
        {
            Local,
            URL
        };
    }

    CHICANE_GRID String toString(Grid::StyleLocationType inValue);
}
