#pragma once

#include <cstdint>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

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

    CHICANE_CORE String toString(Input::MouseButton inValue);
}
