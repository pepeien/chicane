#pragma once

#include "Base.hpp"

namespace Engine
{
    class Pawn;

    class Controller
    {
    public:
        void possess(Pawn* inPawn);
        void unPossess();

        void bindEvent(SDL_Scancode inScaneCode, std::function<void()> inEvent);
        void onEvent(SDL_Scancode inScaneCode);

    private:
        Pawn* m_pawn;
        std::unordered_map<SDL_Scancode, std::function<void()>> events;
    };
}