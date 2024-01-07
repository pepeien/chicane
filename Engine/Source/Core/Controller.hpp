#pragma once

#include "Base.hpp"

class Actor;

namespace Chicane
{
    class Controller
    {
    public:
        void possess(std::shared_ptr<Actor> inActor);

    private:
        std::shared_ptr<Actor> m_character;
    };
}