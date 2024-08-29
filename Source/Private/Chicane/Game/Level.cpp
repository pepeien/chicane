#include "Chicane/Game/Level.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    Level::Level()
        : m_actorObservable(std::make_unique<Observable<Actor*>>()),
        m_meshObservable(std::make_unique<Observable<MeshComponent*>>())
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

    bool Level::hasMeshes() const
    {
        return m_meshes.size() > 0;
    }

    const std::vector<MeshComponent*>& Level::getMeshes() const
    {
        return m_meshes;
    }

    void Level::addMesh(MeshComponent* inMesh)
    {
        if (!inMesh)
        {
            return;
        }

        bool isDuplicate   = std::find(m_meshes.begin(), m_meshes.end(), inMesh) != m_meshes.end();
        bool isDeactivated = !inMesh->isActive();

        if (isDuplicate || isDeactivated)
        {
            return;
        }

        m_meshes.push_back(inMesh);

        m_meshObservable->next(inMesh);
    }

    void Level::watchMeshes(
        std::function<void (MeshComponent*)> inNextCallback,
        std::function<void (const std::string&)> inErrorCallback,
        std::function<void ()> inCompleteCallback
    )
    {
        m_meshObservable->subscribe(
            inNextCallback,
            inErrorCallback,
            inCompleteCallback
        )->next(m_meshes.empty() ? nullptr : m_meshes.back());
    }
}