#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Input
    {
        // Extracted from `SDL3/SDL_MouseButtonEvent`
        enum class MouseButton : std::uint8_t
        {
            Left = 1,
            Middle,
            Right
        };
    }
}