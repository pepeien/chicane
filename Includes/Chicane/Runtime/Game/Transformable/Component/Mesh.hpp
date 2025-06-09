#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CMesh : public Component
    {
    public:
        CMesh();
        virtual ~CMesh() = default;

    protected:
        void onActivation() override;
        void onDeactivation() override;
        void onAttachment(Transformable* inRoot) override;

    public:
        bool isDrawable() const;
        void show();
        void hide();

        bool hasMesh() const;
        void setMesh(const String& inMesh);
        const Box::Mesh::Instance* getMesh() const;
        const String& getModel() const;
        const String& getTexture() const;

    protected:
        void generateBounds();

    protected:
        bool                       m_bIsVisible;

        const Box::Mesh::Instance* m_mesh;
    };
}