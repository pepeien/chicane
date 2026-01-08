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
}