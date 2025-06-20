#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Window/Event/Type.hpp"

namespace Chicane
{
    struct CHICANE_CORE WindowEvent
    {
    public:
        WindowEventType type = WindowEventType::First;
        void*           data = nullptr; // SDL_Event
    };
}