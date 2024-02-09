#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Game/Actor.hpp"

namespace Chicane
{
    class Controller;

    class Pawn : public Actor
    {
    public:
        Pawn();

    public:
        template<class T = Controller>
        T* getController()
        {
            return dynamic_cast<T*>(m_controller);
        }

        bool isPossessed();
        virtual void getPossesedBy(Controller* inController);
        virtual void getUnpossessed();

    protected:
        Controller* m_controller;
    };
}