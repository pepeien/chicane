#include "Chicane/Runtime/Game/Transformable/Component/Mesh.hpp"

#include "Chicane/Runtime/Application.hpp"

const std::string EMPTY_STRING = "";

namespace Chicane
{
    CMesh::CMesh()
        : Component(),
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
        if (!Application::hasLevel())
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

    void CMesh::setMesh(const std::string& inMesh)
    {
        if (inMesh.empty() || hasMesh())
        {
            return;
        }

        m_mesh = Box::load<Box::Mesh::Instance>(inMesh);

        generateBounds();
    }

    const Box::Mesh::Instance* CMesh::getMesh() const
    {
        return m_mesh;
    }

    const std::string& CMesh::getModel() const
    {
        if (!hasMesh())
        {
            return EMPTY_STRING;
        }

        return m_mesh->getGroups().at(0).getModel();
    }

    const std::string& CMesh::getTexture() const
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
            const Box::Model::ParsedData& model = manager->getInstance(group.getModel());

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