#pragma once

#include "Chicane/Box/Mesh.hpp"

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    CH_TYPE(Manual)
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

        bool canCastShadows() const;
        void setCanCastShadows(bool inValue);

        bool isLit() const;
        void setIsLit(bool inValue);

    protected:
        void generateBounds();

    protected:
        bool             m_bIsVisible;
        bool             m_bCanCastShadows;
        bool             m_bIsLit;

        const Box::Mesh* m_asset;
    };
}
