#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Module
    {
        using InitFn     = bool (*)();
        using ShutdownFn = void (*)();

        struct CHICANE_CORE Entry
        {
        public:
            void*      handle   = nullptr; // SDL_SharedObject*
            ShutdownFn shutdown = nullptr;
        };
    }
}
