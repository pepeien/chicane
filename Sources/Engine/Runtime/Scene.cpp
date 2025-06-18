#include "Chicane/Runtime/Scene.hpp"

static inline constexpr const std::uint32_t MAX_ACTOR_COUNT     = 10000;
static inline constexpr const std::uint32_t MAX_COMPONENT_COUNT = MAX_ACTOR_COUNT * 4;

namespace Chicane
{
    Scene::Scene()
        : m_actors({}),
        m_actorObservable({}),
        m_components({}),
        m_componentObservable({}),
        m_defaultCamera(nullptr)
    {
        m_actors.reserve(MAX_ACTOR_COUNT);
        m_components.reserve(MAX_COMPONENT_COUNT);
    }

    Scene::~Scene()
    {
        for (Component* component : m_components)
        {
            delete component;
            component = nullptr;
        }
        m_components.clear();

        for (Actor* actor : m_actors)
        {
            delete actor;
            actor = nullptr;
        }
        m_actors.clear();
    }

    void Scene::activate()
    {
        onActivation();
    }

    void Scene::tick(float inDeltaTime)
    {
        for (Actor* actor : m_actors)
        {
            actor->tick(inDeltaTime);
        }

        for (Component* component : m_components)
        {
            component->tick(inDeltaTime);
        }

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

        m_actorObservable.next(m_actors);

        refreshDefaultCamera();
    }

    Scene::ActorsSubscription Scene::watchActors(
        ActorsSubscription::NextCallback inNext,
        ActorsSubscription::ErrorCallback inError,
        ActorsSubscription::CompleteCallback inComplete
    )
    {
        return m_actorObservable
            .subscribe(inNext, inError, inComplete)
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

        auto found = std::find(m_components.begin(), m_components.end(), inComponent);

        if (found == m_components.end())
        {
            return;
        }

        m_components.erase(found);

        m_componentObservable.next(m_components);

        refreshDefaultCamera();
    }

    Scene::ComponentsSubscription Scene::watchComponents(
        ComponentsSubscription::NextCallback inNext,
        ComponentsSubscription::ErrorCallback inError,
        ComponentsSubscription::CompleteCallback inComplete
    )
    {
        return m_componentObservable
           .subscribe(inNext, inError, inComplete)
           .next(m_components);
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

        m_componentObservable.next(m_components);
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