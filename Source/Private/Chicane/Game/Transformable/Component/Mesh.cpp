#include "Chicane/Game/Transformable/Component/Mesh.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

const std::string EMPTY_STRING = "";

namespace Chicane
{
    CMesh::CMesh(const std::string& inMesh)
        : Component(),
        m_bIsMeshActive(false),
        m_mesh(Loader::loadMesh(inMesh))
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

    void CMesh::onAttachment(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        inActor->setBounds(getBounds());
    }

    void CMesh::onTick(float inDeltaTime)
    {
        //updateVisibility();
    }

    bool CMesh::isDrawable() const
    {
        if (!Application::hasCamera() || !Application::hasLevel())
        {
            return false;
        }

        return hasMesh() && isActive() && m_bIsMeshActive;
    }

    bool CMesh::hasMesh() const
    {
        return !m_mesh->getFilepath().empty();
    }

    const Box::Mesh* CMesh::getMesh() const
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
        const Model::Manager* manager = Loader::getModelManager();

        std::vector<Vertex::Instance> vertices = {};

        for (const Box::Mesh::Group& group : m_mesh->getGroups())
        {
            const Model::Instance&                       model = manager->getInstance(group.getModel());
            const std::vector<Vertex::Instance>& modelVertices = model.vertices;

            for (std::uint32_t index : model.indices)
            {
                vertices.push_back(modelVertices.at(index));
            }
        }

        Bounds bounds = Bounds(vertices);

        setBounds(bounds);

        if (isAttached())
        {
            m_attachment->setBounds(bounds);
        }
    }

    void CMesh::updateVisibility()
    {
        if (!Application::hasCamera())
        {
            return;
        }

        if (Application::getCamera()->canSee(this))
        {
            show();
        }
        else
        {
            hide();
        }
    }

    void CMesh::show()
    {
        if (m_bIsMeshActive)
        {
            return;
        }

        Texture::Manager* textureManager = Loader::getTextureManager();
        Model::Manager* modelManager = Loader::getModelManager();

        for (const auto& group : m_mesh->getGroups())
        {
            textureManager->activate(group.getTexture());
            modelManager->activate(group.getModel());
        }

        m_bIsMeshActive = true;
    }

    void CMesh::hide()
    {
        if (!m_bIsMeshActive)
        {
            return;
        }

        Texture::Manager* textureManager = Loader::getTextureManager();
        Model::Manager* modelManager = Loader::getModelManager();

        for (const auto& group : m_mesh->getGroups())
        {
            textureManager->deactivate(group.getTexture());
            modelManager->deactivate(group.getModel());
        }

        m_bIsMeshActive = false;
    }
}