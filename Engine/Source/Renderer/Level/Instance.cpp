#include "Instance.hpp"

namespace Chicane
{
    namespace Level
    {
        std::vector<Actor::Pawn> Instance::getActors()
        {
            return actors;
        }
    
        void Instance::addActor(const Actor::Pawn& inObject)
        {
            actors.push_back(inObject);
        }
    }
}