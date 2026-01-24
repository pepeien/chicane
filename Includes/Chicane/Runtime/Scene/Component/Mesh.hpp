#pragma once

#include "Chicane/Box/Mesh.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME CMesh : public Component
    {
    public:
        CMesh();
        virtual ~CMesh() = default;

    protected:
        void onAttachment(Transformable* inRoot) override;

    public:
        bool isDrawable() const;

        bool hasMesh() const;
        void setMesh(const String& inMesh);
        const Box::Mesh* getMesh() const;
        const String& getModel() const;
        const String& getTexture() const;

    protected:
        void generateBounds();

    protected:
        bool             m_bIsVisible;

        const Box::Mesh* m_mesh;
    };
}