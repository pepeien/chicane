#pragma once

#include <cstdint>

#include "Chicane/Grid.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class DockSide : std::uint8_t
        {
            Fill,
            Float,
            Left,
            Right,
            Top,
            Bottom
        };
    }

    inline String toString(Grid::DockSide inValue)
    {
        switch (inValue)
        {
        case Grid::DockSide::Fill:
            return "Fill";

        case Grid::DockSide::Float:
            return "Float";

        case Grid::DockSide::Left:
            return "Left";

        case Grid::DockSide::Right:
            return "Right";

        case Grid::DockSide::Top:
            return "Top";

        case Grid::DockSide::Bottom:
            return "Bottom";

        default:
            return "";
        }
    }
}
