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
        void getPossesedBy(Controller* inController);
        void getUnpossessed();

    private:
        Controller* m_controller;
    };
}