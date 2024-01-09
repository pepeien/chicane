#pragma once

#include "Base.hpp"

#include "Actor.hpp"

namespace Engine
{
    class Controller
    {
    private:
        std::shared_ptr<Actor::Pawn> m_pawn;
    };
}