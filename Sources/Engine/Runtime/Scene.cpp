#include "Chicane/Runtime/Scene.hpp"

namespace Chicane
{
    Scene::Scene()
        : m_actors({}),
          m_actorsObservable({}),
          m_components({}),
          m_componentsObservable({}),
          m_defaultCamera(nullptr)
    {}

    Scene::~Scene()
    {
        deleteComponents();
        deleteActors();
    }

    void Scene::activate()
    {
        onActivation();
    }

    void Scene::tick(float inDeltaTime)
    {
        tickActors(inDeltaTime);
        tickComponents(inDeltaTime);

        onTick(inDeltaTime);
    }

    void Scene::deactivate()
    {
        onDeactivation();
    }

    bool Scene::hasActors() const
    {
        return m_actors.size() > 0;
    }

    const std::vector<Actor*>& Scene::getActors() const
    {
        return m_actors;
    }

    void Scene::removeActor(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        auto found = std::find(m_actors.begin(), m_actors.end(), inActor);

        if (found == m_actors.end())
        {
            return;
        }

        m_actors.erase(found);

        m_actorsObservable.next(m_actors);

        refreshDefaultCamera();
    }

    Scene::ActorsSubscription Scene::watchActors(
        ActorsSubscription::NextCallback     inNext,
        ActorsSubscription::ErrorCallback    inError,
        ActorsSubscription::CompleteCallback inComplete
    )
    {
        return m_actorsObservable.subscribe(inNext, inError, inComplete)
            .next(m_actors);
    }

    bool Scene::hasComponents() const
    {
        return m_components.size() > 0;
    }

    const std::vector<Component*>& Scene::getComponents() const
    {
        return m_components;
    }

    void Scene::removeComponent(Component* inComponent)
    {
        if (!inComponent)
        {
            return;
        }

        auto found =
            std::find(m_components.begin(), m_components.end(), inComponent);

        if (found == m_components.end())
        {
            return;
        }

        m_components.erase(found);

        m_componentsObservable.next(m_components);

        refreshDefaultCamera();
    }

    Scene::ComponentsSubscription Scene::watchComponents(
        ComponentsSubscription::NextCallback     inNext,
        ComponentsSubscription::ErrorCallback    inError,
        ComponentsSubscription::CompleteCallback inComplete
    )
    {
        return m_componentsObservable.subscribe(inNext, inError, inComplete)
            .next(m_components);
    }

    void Scene::tickActors(float inDeltaTime)
    {
        for (Actor* actor : m_actors)
        {
            actor->tick(inDeltaTime);
        }
    }

    void Scene::deleteActors()
    {
        for (Actor* actor : m_actors)
        {
            delete actor;
            actor = nullptr;
        }
        m_actors.clear();
    }

    void Scene::tickComponents(float inDeltaTime)
    {
        for (Component* component : m_components)
        {
            component->tick(inDeltaTime);
        }
    }

    void Scene::deleteComponents()
    {
        for (Component* component : m_components)
        {
            delete component;
            component = nullptr;
        }
        m_components.clear();
    }

    void Scene::createDefaultCamera()
    {
        if (m_defaultCamera != nullptr)
        {
            return;
        }

        m_defaultCamera = new CCamera();
        m_defaultCamera->activate();

        m_components.push_back(m_defaultCamera);

        m_componentsObservable.next(m_components);
    }

    void Scene::removeDefaultCamera()
    {
        if (!m_defaultCamera)
        {
            return;
        }

        removeComponent(m_defaultCamera);

        m_defaultCamera = nullptr;
    }

    void Scene::refreshDefaultCamera()
    {
        bool hasUserCamera = false;

        for (CCamera* camera : getComponents<CCamera>())
        {
            if (camera == m_defaultCamera || !camera->isActive())
            {
                continue;
            }

            hasUserCamera = true;

            break;
        }

        if (hasUserCamera)
        {
            removeDefaultCamera();

            return;
        }

        createDefaultCamera();
    }
}