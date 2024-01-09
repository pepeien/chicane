#pragma once

#include "Base.hpp"

namespace Engine
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