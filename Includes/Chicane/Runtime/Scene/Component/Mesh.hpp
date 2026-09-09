#pragma once

#include "Chicane/Box/Mesh.hpp"

#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Renderer/Draw/Poly/3D/Flag.hpp"

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

        Renderer::DrawPoly3DFlag getFlags() const;
        void setFlags(Renderer::DrawPoly3DFlag inValue);

        bool canCastShadows() const;
        void setCanCastShadows(bool inValue);

        bool isLit() const;
        void setIsLit(bool inValue);

        bool isForeground() const;
        void setIsForeground(bool inValue);

        bool isOutlined() const;
        void setIsOutlined(bool inValue);

    protected:
        void generateBounds();

        void setFlag(Renderer::DrawPoly3DFlag inFlag, bool inValue);

    protected:
        bool                     m_bIsVisible;
        Renderer::DrawPoly3DFlag m_flags;

        const Box::Mesh*         m_asset;
    };
}
