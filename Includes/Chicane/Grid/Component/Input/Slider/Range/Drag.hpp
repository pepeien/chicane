#pragma once

#include <cstdint>

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class InputRangeSliderDrag : std::uint8_t
        {
            None,
            Low,
            High,
            Span
        };
    }

    CHICANE_GRID String toString(Grid::InputRangeSliderDrag inValue);
}
