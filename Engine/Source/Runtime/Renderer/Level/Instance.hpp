#pragma once

#include "Base.hpp"

#include "Actor.hpp"

namespace Chicane
{
    namespace Level
    {
        class Instance
        {
        public:
            std::vector<Actor::Instance> getActors();
            void addActor(const Actor::Instance& inObject);

        public:
            std::vector<Actor::Instance> actors;
        };
    }
}