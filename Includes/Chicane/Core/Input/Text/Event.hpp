#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Input
    {
        struct CHICANE_CORE TextEvent
        {
        public:
            TextEvent(void* inData); // From `SDL_TextInputEvent`
            TextEvent();

        public:
            String text;
        };
    }
}
