#pragma once

#include "Base.hpp"

#include "Actor.hpp"

namespace Engine
{
    class Level
    {
    public:
        bool hasSkybox();
        Kerb::Instance getSkybox();
        void setSkybox(const Kerb::Instance& inSkybox);

        std::vector<Actor*> getActors();
        void addActor(Actor* inActor);

    private:
        Kerb::Instance m_skybox;
        std::vector<Actor*> m_actors;
    };
}