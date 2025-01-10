#include "Chicane/Game/Level/Instance.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    Level::Level()
        : m_actorObservable(std::make_unique<Observable<Actor*>>()),
        m_componentObservable(std::make_unique<Observable<const std::vector<Component*>&>>())
    {}

    Level::~Level()
    {
        for (Component* component : m_components)
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

    void Level::onTick(float inDeltaTime)
    {
        for (Actor* actor : m_actors)
        {
            actor->tick(inDeltaTime);
        }

        for (Component* component : m_components)
        {
            component->tick(inDeltaTime);
        }
    }

    void Level::activate()
    {
        Application::setLevel(this);

        onActivation();
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

    Subscription<Actor*>* Level::watchActors(
        std::function<void (Actor*)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        Subscription<Actor*>* subscription = m_actorObservable->subscribe(
            inNext,
            inError,
            inComplete
        );
        subscription->next(m_actors.empty() ? nullptr : m_actors.back());

        return subscription;
    }

    bool Level::hasComponents() const
    {
        return m_components.size() > 0;
    }

    const std::vector<Component*>& Level::getComponents() const
    {
        return m_components;
    }

    void Level::addComponent(Component* inComponent)
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

    void Level::removeComponent(Component* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        auto foundComponent = std::find(m_components.begin(), m_components.end(), inComponent);

        if (foundComponent == m_components.end())
        {
            return;
        }

        m_components.erase(foundComponent);

        m_componentObservable->next(m_components);
    }

    Subscription<const std::vector<Component*>&>* Level::watchComponents(
        std::function<void (const std::vector<Component*>&)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        Subscription<const std::vector<Component*>&>* subscription = m_componentObservable->subscribe(
            inNext,
            inError,
            inComplete
        );
        subscription->next(m_components);

        return subscription;
    }
}