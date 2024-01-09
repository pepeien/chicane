#include "Window.hpp"

namespace Engine
{
    namespace Window
    {
        void init(Instance& outWindow)
        {
            SDL_DisplayMode desktop;
            SDL_GetCurrentDisplayMode(0, &desktop);

            outWindow.width  = desktop.w;
            outWindow.height = desktop.h;

            outWindow.instance = SDL_CreateWindow(
                outWindow.title.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                outWindow.width,
                outWindow.height,
                SDL_WINDOW_RESIZABLE |
                SDL_WINDOW_MAXIMIZED |
                SDL_WINDOW_VULKAN
            );

            if (outWindow.instance == NULL)
            {
                throw std::runtime_error(SDL_GetError());
            }
        }
    }
}