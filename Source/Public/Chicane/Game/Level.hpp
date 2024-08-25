#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Game/Actor.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Chicane
{
    class Level
    {
    public:
        Level();
        ~Level();

    public:
        bool hasActors() const;
        const std::vector<Actor*>& getActors() const;
        void addActor(Actor* inActor);
        void watchActors(
            std::function<void (Actor*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        bool hasMeshes() const;
        const std::vector<MeshComponent*>& getMeshes() const;
        void addMesh(MeshComponent* inMesh);
        void watchMeshes(
            std::function<void (MeshComponent*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

    private:
        std::vector<Actor*> m_actors;
        std::unique_ptr<Observable<Actor*>> m_actorObservable;

        std::vector<MeshComponent*> m_meshes;
        std::unique_ptr<Observable<MeshComponent*>> m_meshObservable;
    };
}