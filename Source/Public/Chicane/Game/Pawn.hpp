#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game/Actor.hpp"

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
        void getPossesedBy(Controller* inController);
        void getDepossessed();

        virtual void onPossession() {};
        virtual void onDepossession() {};

    protected:
        Controller* m_controller;
    };
}