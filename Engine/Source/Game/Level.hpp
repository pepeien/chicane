#pragma once

#include "Base.hpp"

#include "Actor.hpp"

namespace Engine
{
    class Level
    {
    public:
        std::vector<Actor::Default*> getActors();
        void addActor(Actor::Default* inActor);

    private:
        std::vector<Actor::Default*> m_actors;
    };
}