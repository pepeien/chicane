#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        enum class StylePosition : std::uint8_t
        {
            Absolute, // Start from (0, 0) a.k.a Top Left corner
            Relative // Continue from the current ImGui's cursor position
        };
    }
}