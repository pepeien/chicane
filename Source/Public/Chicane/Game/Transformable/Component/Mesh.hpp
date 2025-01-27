#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CMesh : public Component
    {
    public:
        CMesh(const std::string& inMesh);
        virtual ~CMesh() = default;

    protected:
        void onActivation() override;
        void onDeactivation() override;
        void onAttachment(Actor* inActor) override;
        void onTick(float inDeltaTime) override;

    public:
        bool isDrawable() const;

        bool hasMesh() const;
        const Box::Mesh* getMesh() const;
        const std::string& getModel() const;
        const std::string& getTexture() const;

    protected:
        void generateBounds();

        void updateVisibility();
        void show();
        void hide();

    protected:
        bool m_bIsMeshActive;

        const Box::Mesh* m_mesh;
    };
}