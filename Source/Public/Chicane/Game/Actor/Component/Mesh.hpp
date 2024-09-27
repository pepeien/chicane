#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Game/Actor/Component.hpp"

namespace Chicane
{
    class MeshComponent : public ActorComponent
    {
    public:
        MeshComponent();
        virtual ~MeshComponent() = default;

    protected:
        void onActivation() override;
        void onDeactivation() override;
        void onAttachment(Actor* inActor) override;
        void onTick(float inDeltaTime) override;

    public:
        bool isDrawable() const;

        bool hasMesh() const;
        const Box::Instance& getMesh() const;
        void setMesh(const std::string& inMesh);

        const std::string& getModel() const;
        const std::string& getTexture() const;

    protected:
        void handleDrawability();
        void activateMesh();
        void deactivateMesh();

    protected:
        bool m_bIsMeshActive;

        Box::Instance m_mesh;
    };
}