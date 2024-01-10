#include "Level.hpp"

namespace Engine
{
    std::vector<Actor*> Level::getActors()
    {
        return m_actors;
    }

    void Level::addActor(Actor* inActor)
    {
        m_actors.push_back(inActor);
    }
}