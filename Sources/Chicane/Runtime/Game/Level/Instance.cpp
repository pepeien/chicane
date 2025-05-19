#include "Chicane/Runtime/Game/Level/Instance.hpp"

#include "Chicane/Runtime/Application.hpp"

static constexpr std::uint32_t MAX_ACTOR_COUNT     = 10000;
static constexpr std::uint32_t MAX_COMPONENT_COUNT = MAX_ACTOR_COUNT * 4;

namespace Chicane
{
    Level::Level()
        : m_actors({}),
        m_actorObservable(std::make_unique<Observable<const std::vector<Actor*>&>>()),
        m_components({}),
        m_componentObservable(std::make_unique<Observable<const std::vector<Component*>&>>()),
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
        Application::setLevel(this);

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

    Subscription<const std::vector<Actor*>&>* Level::watchActors(
        std::function<void (const std::vector<Actor*>&)> inNext,
        std::function<void (const std::string&)> inError,
        std::function<void ()> inComplete
    )
    {
        Subscription<const std::vector<Actor*>&>* subscription = m_actorObservable->subscribe(
            inNext,
            inError,
            inComplete
        );
        subscription->next(m_actors);

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