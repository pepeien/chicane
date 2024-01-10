#include "Controller.hpp"

namespace Engine
{
    void Controller::bindEvent(SDL_Scancode inScanCode, std::function<void()> inEvent)
    {
        events.insert(
            std::make_pair(
                inScanCode,
                inEvent
            )
        );
    }
}