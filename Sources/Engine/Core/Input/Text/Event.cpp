#include "Chicane/Core/Input/Text/Event.hpp"

#include <SDL3/SDL.h>

namespace Chicane
{
    namespace Input
    {
        TextEvent::TextEvent(void* inData)
            : TextEvent()
        {
            const SDL_TextInputEvent event = *static_cast<SDL_TextInputEvent*>(inData);

            text = event.text ? event.text : "";
        }

        TextEvent::TextEvent()
            : text(String::empty())
        {}
    }
}
