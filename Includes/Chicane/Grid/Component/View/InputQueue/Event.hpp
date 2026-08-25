#pragma once

#include "Chicane/Core/Input/Keyboard/Event.hpp"
#include "Chicane/Core/Input/Mouse/Button/Event.hpp"
#include "Chicane/Core/Input/Mouse/Motion/Event.hpp"
#include "Chicane/Core/Input/Mouse/Wheel/Event.hpp"
#include "Chicane/Core/Input/Text/Event.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID ViewInputQueueEvent
        {
        public:
            WindowEventType         type     = WindowEventType::First;
            Input::MouseMotionEvent motion   = {};
            Input::MouseButtonEvent button   = {};
            Input::MouseWheelEvent  wheel    = {};
            Input::KeyboardEvent    keyboard = {};
            Input::TextEvent        text     = {};
        };
    }
}