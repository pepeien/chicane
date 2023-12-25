#include "Instance.hpp"

namespace Chicane
{
    namespace Level
    {
        std::vector<Actor::Instance> Instance::getActors()
        {
            return actors;
        }
    
        void Instance::addActor(const Actor::Instance& inObject)
        {
            actors.push_back(inObject);
        }
    }
}