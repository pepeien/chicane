#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Window/EventType.hpp"

namespace Chicane
{
    namespace Window
    {
        struct CHICANE_CORE Event
        {
        public:
            EventType type = EventType::First;
            void*     data = nullptr; // SDL_Event
        };
    }
}