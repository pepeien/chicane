#pragma once

#include <functional>

#include "Base.hpp"

namespace Engine
{
    class Pawn;

    class Controller
    {
    public:
        void bindEvent(SDL_Scancode inScaneCode, std::function<void()> inEvent);

    private:
        Pawn* m_pawn;
        std::unordered_map<SDL_Scancode, std::function<void()>> events;
    };
}