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

    protected:
        void onAttachment(Object* inRoot) override;

    public:
        bool isDrawable() const;

        bool hasMesh() const;
        void setMesh(const FileSystem::Path& inFilePath);
        const Box::Mesh* getMesh() const;

    protected:
        void generateBounds();

    protected:
        bool             m_bIsVisible;

        const Box::Mesh* m_asset;
    };
}