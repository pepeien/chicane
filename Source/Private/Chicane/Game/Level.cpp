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

    bool Level::hasActors() const
    {
        return m_actors.size() > 0;
    }

    const std::vector<Actor*>& Level::getActors() const
    {
        return m_actors;
    }

    std::vector<Actor*> Level::getDrawableActors() const
    {
        std::vector<Actor*> result {};

        for (Actor* actor : m_actors)
        {
            if (!actor->hasMesh())
            {
                continue;
            }

            result.push_back(actor);
        }

        return result;
    }

    std::uint32_t Level::getActorCount() const
    {
        return static_cast<std::uint32_t>(m_actors.size());
    }

    void Level::addActor(Actor* inActor)
    {
        if (inActor == nullptr)
        {
            return;
        }

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