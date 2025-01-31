#include "Chicane/Game/Transformable/Component/Mesh.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core.hpp"

const std::string EMPTY_STRING = "";

namespace Chicane
{
    CMesh::CMesh(const std::string& inMesh)
        : Component(),
        m_bIsVisible(false),
        m_mesh(Box::loadMesh(inMesh))
    {
        generateBounds();
    }

    void CMesh::onActivation()
    {
        if (!Application::hasLevel())
        {
            return;
        }

        show();
    }

    void CMesh::onDeactivation()
    {
        if (!Application::hasLevel())
        {
            return;
        }

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

    void CMesh::onTick(float inDeltaTime)
    {
        refresh();
    }

    bool CMesh::isDrawable() const
    {
        if (!Application::hasCamera() || !Application::hasLevel())
        {
            return false;
        }

        return hasMesh() && isActive() && m_bIsVisible;
    }

    bool CMesh::hasMesh() const
    {
        return !m_mesh->getFilepath().empty();
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
        const Box::Model::Manager* manager = Box::getModelManager();

        std::vector<Box::Model::Vertex> vertices = {};

        for (const Box::Mesh::Group& group : m_mesh->getGroups())
        {
            const Box::Model::RawData& model = manager->getInstance(group.getModel());

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

    void CMesh::refresh()
    {
        if (!Application::hasCamera())
        {
            return;
        }

        if (Application::getCamera()->canSee(this))
        {
            //show();
        }
        else
        {
            //hide();
        }
    }

    void CMesh::show()
    {
        if (m_bIsVisible)
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
        if (!m_bIsVisible)
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
}