#include "Level.hpp"

namespace Engine
{
    std::vector<Actor::Default*> Level::getActors()
    {
        return m_actors;
    }

    void Level::addActor(Actor::Default* inActor)
    {
        m_actors.push_back(inActor);
    }
}