#pragma once

#include "Core.hpp"

namespace Chicane
{
    namespace Controller
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
}