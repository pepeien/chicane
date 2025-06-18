#include "Chicane/Runtime/Scene/Component/Mesh.hpp"

#include "Chicane/Runtime/Application.hpp"

namespace Chicane
{
    static const String EMPTY_STRING = "";

    CMesh::CMesh()
        : Super(),
        m_bIsVisible(false),
        m_mesh(nullptr)
    {}

    void CMesh::onActivation()
    {
        show();
    }

    void CMesh::onDeactivation()
    {
        hide();
    }

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
        if (!Application::hasScene())
        {
            return false;
        }

        return hasMesh() && isActive() && m_bIsVisible;
    }

    void CMesh::show()
    {
        if (m_bIsVisible || !hasMesh())
        {
            return;
        }

        Box::Model::Manager* modelManager     = Box::getModelManager();
        Box::Texture::Manager* textureManager = Box::getTextureManager();

        for (const auto& group : m_mesh->getGroups())
        {
            modelManager->activate(group.getModel());
            textureManager->activate(group.getTexture());
        }

        m_bIsVisible = true;
    }

    void CMesh::hide()
    {
        if (!m_bIsVisible  || !hasMesh())
        {
            return;
        }

        Box::Model::Manager* modelManager     = Box::getModelManager();
        Box::Texture::Manager* textureManager = Box::getTextureManager();

        for (const auto& group : m_mesh->getGroups())
        {
            modelManager->deactivate(group.getModel());
            textureManager->deactivate(group.getTexture());
        }

        m_bIsVisible = false;
    }

    bool CMesh::hasMesh() const
    {
        return m_mesh && !m_mesh->getFilepath().empty();
    }

    void CMesh::setMesh(const String& inMesh)
    {
        if (inMesh.isEmpty() || hasMesh())
        {
            return;
        }

        m_mesh = Box::load<Box::Mesh::Instance>(inMesh.toStandard());

        generateBounds();
    }

    const Box::Mesh::Instance* CMesh::getMesh() const
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

        const Box::Model::Manager* manager = Box::getModelManager();

        std::vector<Vertex> vertices = {};

        for (const Box::Mesh::Group& group : m_mesh->getGroups())
        {
            const Box::Model::Extracted& model = manager->getInstance(group.getModel());

            vertices.insert(
                vertices.end(),
                model.vertices.begin(),
                model.vertices.end()
            );
        }

        Bounds bounds = Bounds(vertices);

        setBounds(bounds);

        if (isAttached())
        {
            m_attachment->setBounds(bounds);
        }
    }
}