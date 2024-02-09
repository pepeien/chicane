#pragma once

#include "Runtime/Runtime.hpp"

namespace Chicane
{
    class Pawn;

    class Controller
    {
    public:
        Controller();

    public:
        template<class T = Pawn>
        T* getPawn()
        {
            return dynamic_cast<T*>(m_pawn);
        }

        void possess(Pawn* inPawn);
        void unPossess();

        void bindEvent(SDL_Scancode inScanCode, std::function<void()> inEvent);
        void onEvent(SDL_Scancode inScanCode);

    private:
        Pawn* m_pawn;
        std::unordered_map<SDL_Scancode, std::function<void()>> events;
    };
}