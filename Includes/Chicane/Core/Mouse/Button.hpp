#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Mouse
    {
        enum class Button : std::uint8_t
        {
            Left   = SDL_BUTTON_LEFT,
            Middle = SDL_BUTTON_MIDDLE,
            Right  = SDL_BUTTON_RIGHT
        };
    }
}