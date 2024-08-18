#include "Chicane/Game/Level.hpp"

namespace Chicane
{
    Level::Level()
        : m_observable(new Observable<Actor*>())
    {}

    Level::~Level()
    {
        for (Actor* actor : m_actors)
        {
            delete actor;
        }

        m_actors.clear();
    }

    bool Level::hasActors()
    {
        return m_actors.size() > 0;
    }

    std::vector<Actor*> Level::getActors()
    {
        return m_actors;
    }

    std::uint32_t Level::getActorCount()
    {
        return m_actors.size();
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