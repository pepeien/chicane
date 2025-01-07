#include "Chicane/Game/Transformable/Component/MeshComponent.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

const std::string EMPTY_STRING = "";

namespace Chicane
{
    MeshComponent::MeshComponent()
        : Component(),
        m_bIsMeshActive(false)
    {}

    void MeshComponent::onActivation()
    {
        if (!Application::hasLevel())
        {
            return;
        }

        show();

        Application::getLevel()->addComponent(this);
    }

    void MeshComponent::onDeactivation()
    {
        if (!Application::hasLevel())
        {
            return;
        }

        hide();

        Application::getLevel()->removeComponent(this);
    }

    void MeshComponent::onAttachment(Actor* inActor)
    {
        if (!inActor)
        {
            return;
        }

        inActor->setBounds(getBounds());
    }

    void MeshComponent::onTick(float inDeltaTime)
    {
        //updateVisibility();
    }

    bool MeshComponent::isDrawable() const
    {
        if (!Application::hasCamera() || !Application::hasLevel())
        {
            return false;
        }

        return hasMesh() && isActive() && m_bIsMeshActive;
    }

    bool MeshComponent::hasMesh() const
    {
        return !m_mesh->getFilepath().empty();
    }

    const Box::Mesh* MeshComponent::getMesh() const
    {
        return m_mesh;
    }

    void MeshComponent::setMesh(const std::string& inMesh)
    {
        m_mesh = Loader::loadMesh(inMesh);

        generateBounds();
    }

    const std::string& MeshComponent::getModel() const
    {
        if (!hasMesh())
        {
            return EMPTY_STRING;
        }

        return m_mesh->getGroups().at(0).getModel();
    }

    const std::string& MeshComponent::getTexture() const
    {
        if (!hasMesh())
        {
            return EMPTY_STRING;
        }

        return m_mesh->getGroups().at(0).getTexture();
    }

    void MeshComponent::generateBounds()
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

    void MeshComponent::updateVisibility()
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

    void MeshComponent::show()
    {
        if (m_bIsMeshActive)
        {
            return;
        }

        Model::Manager* manager = Loader::getModelManager();

        for (const auto& group : m_mesh->getGroups())
        {
            manager->activate(group.getModel());
        }

        m_bIsMeshActive = true;
    }

    void MeshComponent::hide()
    {
        if (!m_bIsMeshActive)
        {
            return;
        }

        Loader::getModelManager()->deactivate(
            m_mesh->getGroups().at(0).getModel()
        );

        m_bIsMeshActive = false;
    }
}