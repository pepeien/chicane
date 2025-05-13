#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Style
        {
            enum class Position : std::uint8_t
            {
                Absolute, // Start from (0, 0) a.k.a Top Left corner
                Relative // Continue from the current ImGui's cursor position
            };
        }
    }
}