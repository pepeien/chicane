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
        typedef std::vector<MouseMotionEventFunction> MouseMotionEvents;

        typedef SDL_MouseButtonEvent MouseButtonEvent;
        typedef std::function<void()> MouseButtonEventFunction;
        typedef std::unordered_map<EventStatus, std::vector<MouseButtonEventFunction>> MouseButtonEventMap;
        typedef std::unordered_map<MouseButton, MouseButtonEventMap> MouseButtonEvents;
    }
}