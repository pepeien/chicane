#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StyleAlignment : std::uint8_t
        {
            Start,
            Center,
            End
        };

        StyleAlignment toAlignment(const String& inText);
    }

    inline String toString(Grid::StyleAlignment inValue)
    {
        switch (inValue)
        {
        case Grid::StyleAlignment::Start:
            return "Start";

        case Grid::StyleAlignment::Center:
            return "Center";

        case Grid::StyleAlignment::End:
            return "End";

        default:
            return "";
        }
    }
}
