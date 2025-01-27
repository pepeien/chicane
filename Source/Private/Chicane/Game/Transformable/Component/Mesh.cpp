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

        Bounds result {};

        for (const Box::Mesh::Group& group : m_mesh->getGroups())
        {
            const Model::Instance& model = manager->getInstance(group.getModel());

            const Vec<3, float>& extent = model.bounds.extent;

            result.extent.x = std::max(
                result.extent.x,
                extent.x
            );
            result.extent.y = std::max(
                result.extent.y,
                extent.y
            );
            result.extent.z = std::max(
                result.extent.z,
                extent.z
            );
        }

        setBounds(result);

        if (isAttached())
        {
            m_attachment->setBounds(result);
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