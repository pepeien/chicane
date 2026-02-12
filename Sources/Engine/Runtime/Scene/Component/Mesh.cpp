#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

#include "Chicane/Box/Model/Manager.hpp"
#include "Chicane/Box/Model/Parsed.hpp"

namespace Chicane
{
    CMesh::CMesh()
        : Component(),
          m_bIsVisible(false),
          m_mesh(nullptr)
    {}

    void CMesh::onAttachment(Transformable* inRoot)
    {
        if (!inRoot)
        {
            return;
        }

        inRoot->setBounds(getBounds());
    }

    bool CMesh::isDrawable() const
    {
        return hasMesh() && isActive() && m_bIsVisible;
    }

    bool CMesh::hasMesh() const
    {
        return m_mesh != nullptr;
    }

    void CMesh::setMesh(const FileSystem::Path& inFilePath)
    {
        if (inFilePath.empty())
        {
            return;
        }

        m_mesh = Box::load<Box::Mesh>(inFilePath);

        generateBounds();
    }

    const Box::Mesh* CMesh::getMesh() const
    {
        return m_mesh;
    }

    void CMesh::generateBounds()
    {
        if (!hasMesh())
        {
            return;
        }

        const Box::ModelManager* manager = Box::getModelManager();

        for (const Box::MeshGroup& group : m_mesh->getGroups())
        {
            const Box::ModelParsed& model = manager->get(group.getModel().getReference());

            const Bounds3D bounds = Bounds3D(model.vertices, model.indices);

            addBounds(bounds);

            if (!isAttached())
            {
                continue;
            }

            m_parent->addBounds(bounds);
        }
    }
}