#include "Chicane/Runtime/Scene/Component/Mesh.reflected.hpp"

#include "Chicane/Box/Model.hpp"

namespace Chicane
{
    CMesh::CMesh()
        : Component(),
          m_bIsVisible(false),
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

    void CMesh::generateBounds()
    {
        if (!hasMesh())
        {
            return;
        }

        for (const Box::MeshGroup& group : m_asset->getGroups())
        {
            const Box::Model* model = Box::load<Box::Model>(group.getModel().getSource());

            addBounds(Bounds3D(model->getModel(group.getModel().getReference()).vertices));
        }

        if (!isAttached())
        {
            return;
        }

        m_parent->addBounds(getBounds());
    }
}