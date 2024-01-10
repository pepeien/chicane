#pragma once

#include "Base.hpp"

#include "Actor.hpp"

namespace Engine
{
    class Level
    {
    public:
        std::vector<Actor*> getActors();
        void addActor(Actor* inActor);

    private:
        std::vector<Actor*> m_actors;
    };
}