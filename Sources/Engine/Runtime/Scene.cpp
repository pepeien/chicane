#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    Scene::Scene()
        : m_bIsLoaded(false),
          m_actorCount(0),
          m_actors({}),
          m_actorsObservable({}),
          m_componentCount(0),
          m_components({}),
          m_componentsObservable({})
    {}

    Scene::~Scene()
    {
        deleteComponents();
        deleteActors();
    }

    void Scene::load()
    {
        onLoad();

        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                actor->onLoad();
            }
        }

        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                component->onLoad();
            }
        }

        m_bIsLoaded = true;
    }

    void Scene::unload()
    {
        m_bIsLoaded = false;

        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                component->onUnload();
            }
        }

        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                actor->onUnload();
            }
        }

        onUnload();
    }

    void Scene::tick(float inDeltaTime)
    {
        tickActors(inDeltaTime);
        tickComponents(inDeltaTime);

        onTick(inDeltaTime);
    }

    bool Scene::hasActors() const
    {
        return m_actorCount > 0;
    }

    std::vector<Actor*> Scene::getActors() const
    {
        std::vector<Actor*> result;
        result.reserve(m_actorCount);

        for (const auto& [type, actors] : m_actors)
        {
            result.insert(result.end(), actors.begin(), actors.end());
        }

        return result;
    }

    void Scene::removeActor(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        auto typed = m_actors.find(std::type_index(typeid(*inActor)));
        if (typed == m_actors.end())
        {
            return;
        }

        auto found = std::find(typed->second.begin(), typed->second.end(), inActor);
        if (found == typed->second.end())
        {
            return;
        }

        typed->second.erase(found);
        m_actorCount--;

        if (typed->second.empty())
        {
            m_actors.erase(typed);
        }

        if (!m_actorsObservable.isEmpty())
        {
            m_actorsObservable.next(getActors());
        }
    }

    Scene::ActorsSubscription Scene::watchActors(
        ActorsSubscription::NextCallback     inNext,
        ActorsSubscription::ErrorCallback    inError,
        ActorsSubscription::CompleteCallback inComplete
    )
    {
        return m_actorsObservable.subscribe(inNext, inError, inComplete).next(getActors());
    }

    bool Scene::hasComponents() const
    {
        return m_componentCount > 0;
    }

    std::vector<Component*> Scene::getComponents() const
    {
        std::vector<Component*> result;
        result.reserve(m_componentCount);

        for (const auto& [type, components] : m_components)
        {
            result.insert(result.end(), components.begin(), components.end());
        }

        return result;
    }

    void Scene::removeComponent(Component* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        auto typed = m_components.find(std::type_index(typeid(*inComponent)));
        if (typed == m_components.end())
        {
            return;
        }

        auto found = std::find(typed->second.begin(), typed->second.end(), inComponent);
        if (found == typed->second.end())
        {
            return;
        }

        typed->second.erase(found);
        m_componentCount--;

        if (typed->second.empty())
        {
            m_components.erase(typed);
        }

        if (!m_componentsObservable.isEmpty())
        {
            m_componentsObservable.next(getComponents());
        }
    }

    Scene::ComponentsSubscription Scene::watchComponents(
        ComponentsSubscription::NextCallback     inNext,
        ComponentsSubscription::ErrorCallback    inError,
        ComponentsSubscription::CompleteCallback inComplete
    )
    {
        return m_componentsObservable.subscribe(inNext, inError, inComplete).next(getComponents());
    }

    bool Scene::isLoaded() const
    {
        return m_bIsLoaded;
    }

    void Scene::tickActors(float inDeltaTime)
    {
        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                actor->tick(inDeltaTime);
            }
        }
    }

    void Scene::deleteActors()
    {
        for (auto& [type, actors] : m_actors)
        {
            for (Actor* actor : actors)
            {
                delete actor;
                actor = nullptr;
            }
        }

        m_actors.clear();
        m_actorCount = 0;
    }

    void Scene::tickComponents(float inDeltaTime)
    {
        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                component->tick(inDeltaTime);
            }
        }
    }

    void Scene::deleteComponents()
    {
        for (auto& [type, components] : m_components)
        {
            for (Component* component : components)
            {
                delete component;
                component = nullptr;
            }
        }

        m_components.clear();
        m_componentCount = 0;
    }
}
