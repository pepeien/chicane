#include "Chicane/Runtime/Game/Level/Instance.hpp"

static constexpr inline const std::uint32_t MAX_ACTOR_COUNT     = 10000;
static constexpr inline const std::uint32_t MAX_COMPONENT_COUNT = MAX_ACTOR_COUNT * 4;

namespace Chicane
{
    Level::Level()
        : m_actors({}),
        m_actorObservable(std::make_unique<ActorsObservable>()),
        m_components({}),
        m_componentObservable(std::make_unique<ComponentsObservable>()),
        m_defaultCamera(nullptr)
    {
        m_actors.reserve(MAX_ACTOR_COUNT);
        m_components.reserve(MAX_COMPONENT_COUNT);
    }

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

    void Level::activate()
    {
        onActivation();
    }

    void Level::tick(float inDeltaTime)
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

    void Level::deactivate()
    {
        onDeactivation();
    }

    bool Level::hasActors() const
    {
        return m_actors.size() > 0;
    }

    const std::vector<Actor*>& Level::getActors() const
    {
        return m_actors;
    }

    void Level::removeActor(Actor* inActor)
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

        m_actorObservable->next(m_actors);

        refreshDefaultCamera();
    }

    Level::ActorsSubscription* Level::watchActors(
        ActorsSubscription::NextCallback inNext,
        ActorsSubscription::ErrorCallback inError,
        ActorsSubscription::CompleteCallback inComplete
    )
    {
        return m_actorObservable
            ->subscribe(inNext, inError, inComplete)
            ->next(m_actors);
    }

    bool Level::hasComponents() const
    {
        return m_components.size() > 0;
    }

    const std::vector<Component*>& Level::getComponents() const
    {
        return m_components;
    }

    void Level::removeComponent(Component* inComponent)
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

        m_componentObservable->next(m_components);

        refreshDefaultCamera();
    }

    Level::ComponentsSubscription* Level::watchComponents(
        ComponentsSubscription::NextCallback inNext,
        ComponentsSubscription::ErrorCallback inError,
        ComponentsSubscription::CompleteCallback inComplete
    )
    {
        return m_componentObservable
            ->subscribe(inNext, inError, inComplete)
            ->next(m_components);
    }

    void Level::createDefaultCamera()
    {
        if (m_defaultCamera != nullptr)
        {
            return;
        }

        m_defaultCamera = new CCamera();
        m_defaultCamera->activate();

        m_components.push_back(m_defaultCamera);

        m_componentObservable->next(m_components);
    }

    void Level::removeDefaultCamera()
    {
        if (!m_defaultCamera)
        {
            return;
        }

        removeComponent(m_defaultCamera);

        m_defaultCamera = nullptr;
    }

    void Level::refreshDefaultCamera()
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