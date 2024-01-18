#include "Level.hpp"

namespace Engine
{
    bool Level::hasSkybox()
    {
        return m_skybox.type == static_cast<uint8_t>(Kerb::Type::CubeMap);
    }

    Kerb::Instance Level::getSkybox()
    {
        return m_skybox;
    }

    void Level::setSkybox(const Kerb::Instance& inAsset)
    {
        if (inAsset.type != static_cast<uint8_t>(Kerb::Type::CubeMap))
        {
            throw std::runtime_error("Failed to set skybox, asset is not a cubemap");
        }

        m_skybox = inAsset;
    }

    std::vector<Actor*> Level::getActors()
    {
        return m_actors;
    }

    void Level::addActor(Actor* inActor)
    {
        m_actors.push_back(inActor);
    }
}