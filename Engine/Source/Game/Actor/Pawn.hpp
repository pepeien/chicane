#pragma once

#include "Base.hpp"

#include "Default.hpp"

class Controller;

namespace Engine
{
    namespace Actor
    {
        class Pawn : public Default
        {
        private:
            std::shared_ptr<Controller> m_controller;
        };
    }
}