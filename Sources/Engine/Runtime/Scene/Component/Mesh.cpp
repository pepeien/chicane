#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

#include "Chicane/Box/Model/Manager.hpp"
#include "Chicane/Box/Model/Parsed.hpp"

namespace Chicane
{
    static const String EMPTY_STRING = "";

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

    void CMesh::setMesh(const String& inMesh)
    {
        if (inMesh.isEmpty() || hasMesh())
        {
            return;
        }

        m_mesh = Box::load<Box::Mesh>(inMesh.toStandard());

        generateBounds();
    }

    const Box::Mesh* CMesh::getMesh() const
    {
        return m_mesh;
    }

    const String& CMesh::getModel() const
    {
        if (!hasMesh())
        {
            return EMPTY_STRING;
        }

        return m_mesh->getGroups().at(0).getModel();
    }

    const String& CMesh::getTexture() const
    {
        if (!hasMesh())
        {
            return EMPTY_STRING;
        }

        return m_mesh->getGroups().at(0).getTexture();
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
            const Box::ModelParsed model = manager->get(group.getModel());

            const Bounds bounds = Bounds(model.vertices, model.indices);

            addBounds(bounds);

            if (!isAttached())
            {
                continue;
            }

            m_parent->addBounds(bounds);
        }
    }
}