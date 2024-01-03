#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Window
    {
        struct Instance
        {
            SDL_Window* instance;
            std::string title;
            int width;
            int height;
            bool isMinimized;
        };

        void init(Instance& outWindow);
    }
}