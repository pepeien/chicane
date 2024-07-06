#include "Runtime/Game/Level.hpp"

namespace Chicane
{
    Level::Level()
        : m_observable(new Observable<Actor*>())
    {}

    bool Level::hasSkybox()
    {
        return m_skybox.type == static_cast<uint8_t>(Box::Type::CubeMap);
    }

    Box::Instance Level::getSkybox()
    {
        return m_skybox;
    }

    void Level::setSkybox(const Box::Instance& inAsset)
    {
        if (inAsset.type != static_cast<uint8_t>(Box::Type::CubeMap))
        {
            throw std::runtime_error("Failed to set skybox, asset is not a cubemap");
        }

        m_skybox = inAsset;
    }

    bool Level::hasActors()
    {
        return m_actors.size() > 0;
    }

    std::vector<Actor*> Level::getActors()
    {
        return m_actors;
    }

    void Level::addActor(Actor* inActor)
    {
        m_actors.push_back(inActor);

        m_observable->next(inActor);
    }

    Subscription<Actor*>* Level::addActorSubscription(
        std::function<void (Actor*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        return m_observable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        );
    }
}