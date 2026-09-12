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

        inline String toString(StyleAlignment inValue)
        {
            switch (inValue)
            {
            case StyleAlignment::Start:
                return "Start";

            case StyleAlignment::Center:
                return "Center";

            case StyleAlignment::End:
                return "End";

            default:
                return "";
            }
        }

        StyleAlignment toAlignment(const String& inText);
    }
}