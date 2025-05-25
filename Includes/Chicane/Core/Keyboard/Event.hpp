#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    namespace Keyboard
    {
        using EventData = SDL_KeyboardEvent;
        using Event     = std::function<void()>;
    }
}