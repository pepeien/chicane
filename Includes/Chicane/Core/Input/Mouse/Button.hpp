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

        inline String toString(MouseButton inValue)
        {
            switch (inValue)
            {
            case MouseButton::Left:
                return "Left";

            case MouseButton::Middle:
                return "Middle";

            case MouseButton::Right:
                return "Right";

            default:
                return "";
            }
        }
    }
}