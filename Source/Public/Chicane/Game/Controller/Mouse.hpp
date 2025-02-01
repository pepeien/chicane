#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Controller/Event.hpp"

namespace Chicane
{
    namespace Controller
    {
        enum class MouseButton : std::uint8_t
        {
            Left   = SDL_BUTTON_LEFT,
            Middle = SDL_BUTTON_MIDDLE,
            Right  = SDL_BUTTON_RIGHT
        };

        typedef SDL_MouseMotionEvent MouseMotionEvent;
        typedef std::function<void(const MouseMotionEvent&)> MouseMotionEventFunction;

        typedef std::function<void()> MouseButtonEventFunction;
    }
}