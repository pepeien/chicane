#pragma once

#include "Base.hpp"

#include "Actor.hpp"

class Controller;

namespace Engine
{
    class Pawn : public Actor
    {
    private:
        std::shared_ptr<Controller> m_controller;
    };
}