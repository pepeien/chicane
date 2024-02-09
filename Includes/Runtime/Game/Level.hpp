#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Game/Actor.hpp"

namespace Chicane
{
    class Level
    {
    public:
        bool hasSkybox();
        Box::Instance getSkybox();
        void setSkybox(const Box::Instance& inSkybox);

        bool hasActors();
        std::vector<Actor*> getActors();
        void addActor(Actor* inActor);

    private:
        Box::Instance m_skybox;
        std::vector<Actor*> m_actors;
    };
}