#pragma once

#include "Base.hpp"

#include "Actor/Pawn.hpp"

namespace Chicane
{
    namespace Level
    {
        class Instance
        {
        public:
            std::vector<Actor::Pawn> getActors();
            void addActor(const Actor::Pawn& inObject);

        private:
            std::vector<Actor::Pawn> actors;
        };
    }
}