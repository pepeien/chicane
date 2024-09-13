#include "Chicane/Game/Level.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    Level::Level()
        : m_actorObservable(std::make_unique<Observable<Actor*>>()),
        m_componentObservable(std::make_unique<Observable<const std::vector<ActorComponent*>&>>())
    {}

    Level::~Level()
    {
        for (ActorComponent* component : m_components)
        {
            delete component;
        }
        m_components.clear();

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

    void Level::addActor(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        if (std::find(m_actors.begin(), m_actors.end(), inActor) != m_actors.end())
        {
            return;
        }

        m_actors.push_back(inActor);

        m_actorObservable->next(inActor);
    }

    void Level::watchActors(
        std::function<void (Actor*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_actorObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_actors.empty() ? nullptr : m_actors.back());
    }

    bool Level::hasComponents() const
    {
        return m_components.size() > 0;
    }

    const std::vector<ActorComponent*>& Level::getComponents() const
    {
        return m_components;
    }

    void Level::addComponent(ActorComponent* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        if (std::find(m_components.begin(), m_components.end(), inComponent) != m_components.end())
        {
            return;
        }

        m_components.push_back(inComponent);

        m_componentObservable->next(m_components);
    }

    void Level::watchComponents(
        std::function<void (const std::vector<ActorComponent*>&)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_componentObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_components);
    }
}