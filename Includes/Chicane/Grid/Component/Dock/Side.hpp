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

        inline String toString(DockSide inValue)
        {
            switch (inValue)
            {
            case DockSide::Fill:
                return "Fill";

            case DockSide::Float:
                return "Float";

            case DockSide::Left:
                return "Left";

            case DockSide::Right:
                return "Right";

            case DockSide::Top:
                return "Top";

            case DockSide::Bottom:
                return "Bottom";

            default:
                return "";
            }
        }
    }
}
