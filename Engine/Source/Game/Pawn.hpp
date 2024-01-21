#pragma once

#include "Base.hpp"

#include "Actor.hpp"

namespace Engine
{
    class Controller;

    class Pawn : public Actor
    {
    public:
        Controller* getController();

        bool isPossessed();
        virtual void getPossesedBy(Controller* inController);
        virtual void getUnpossessed();

    protected:
        Controller* m_controller;
    };
}