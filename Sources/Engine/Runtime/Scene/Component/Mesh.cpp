#include "Chicane/Runtime/Scene/Component/Mesh.reflected.hpp"

#include <unordered_map>

#include "Chicane/Box/Model.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

namespace Chicane
{
    CMesh::CMesh()
        : Component(),
          m_bIsVisible(false),
          m_flags(Renderer::DrawPoly3DFlag::Lit | Renderer::DrawPoly3DFlag::Shadow),
          m_asset(nullptr)
    {}

    void CMesh::onAttachment(Object* inParent)
    {
        if (!inParent)
        {
            return;
        }

        inParent->addBounds(getBounds());
    }

    bool CMesh::isDrawable() const
    {
        return hasMesh() && isActive() && m_bIsVisible;
    }

    bool CMesh::hasMesh() const
    {
        return m_asset != nullptr;
    }

    void CMesh::setMesh(const FileSystem::Path& inFilePath)
    {
        if (inFilePath.isEmpty())
        {
            return;
        }

        m_asset = Box::load<Box::Mesh>(inFilePath);

        generateBounds();
    }

    const Box::Mesh* CMesh::getMesh() const
    {
        return m_asset;
    }

    Renderer::DrawPoly3DFlag CMesh::getFlags() const
    {
        return m_flags;
    }

    void CMesh::setFlags(Renderer::DrawPoly3DFlag inValue)
    {
        m_flags = inValue;
    }

    bool CMesh::canCastShadows() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Shadow);
    }

    void CMesh::setCanCastShadows(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Shadow, inValue);
    }

    bool CMesh::isLit() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Lit);
    }

    void CMesh::setIsLit(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Lit, inValue);
    }

    bool CMesh::isForeground() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Foreground);
    }

    void CMesh::setIsForeground(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Foreground, inValue);
    }

    bool CMesh::isOutlined() const
    {
        return Renderer::has(m_flags, Renderer::DrawPoly3DFlag::Outlined);
    }

    void CMesh::setIsOutlined(bool inValue)
    {
        setFlag(Renderer::DrawPoly3DFlag::Outlined, inValue);
    }

    void CMesh::setFlag(Renderer::DrawPoly3DFlag inFlag, bool inValue)
    {
        if (inValue)
        {
            m_flags |= inFlag;

            return;
        }

        m_flags &= ~inFlag;
    }

    void CMesh::generateBounds()
    {
        if (!hasMesh())
        {
            return;
        }

        static std::unordered_map<const Box::Mesh*, Bounds3D> boundsByMesh;

        auto cached = boundsByMesh.find(m_asset);
        if (cached == boundsByMesh.end())
        {
            Bounds3D bounds;

            for (const Box::MeshGroup& group : m_asset->getGroups())
            {
                const Box::Model* model = Box::load<Box::Model>(group.getModel().getSource());

                if (!model)
                {
                    model = Box::Model::getDefault();
                }

                if (!model)
                {
                    continue;
                }

                const Vertex::List& vertices = model->getModel(group.getModel().getReference()).vertices;
                const Mat4&         local    = group.getModelMatrix();

                Vertex::List transformed;
                transformed.reserve(vertices.size());
                for (const Vertex& vertex : vertices)
                {
                    Vertex copy   = vertex;
                    copy.position = local * vertex.position;
                    transformed.push_back(copy);
                }

                bounds.add(transformed);
            }

            cached = boundsByMesh.emplace(m_asset, std::move(bounds)).first;
        }

        addBounds(cached->second);

        if (!isAttached())
        {
            return;
        }

        m_parent->addBounds(getBounds());
    }
}
