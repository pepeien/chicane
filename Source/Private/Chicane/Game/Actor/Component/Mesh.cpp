#include "Chicane/Game/Actor/Component/Mesh.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game/Actor/Component/Camera.hpp"

namespace Chicane
{
    MeshComponent::MeshComponent()
        : ActorComponent(),
        m_isMeshActive(false)
    {
        addComponent(this);
    }

    void MeshComponent::onActivation()
    {
        if (!hasActiveLevel())
        {
            return;
        }

        activateMesh();
    }

    void MeshComponent::onDeactivation()
    {
        if (!hasActiveLevel())
        {
            return;
        }

        deactivateMesh();
    }

    bool MeshComponent::isDrawable() const
    {
        if (!hasActiveCamera() || !hasActiveLevel())
        {
            return false;
        }

        return hasMesh() && isActive() && m_isMeshActive;
    }

    bool MeshComponent::hasMesh() const
    {
        return !m_mesh.filepath.empty();
    }

    const Box::Instance& MeshComponent::getMesh() const
    {
        return m_mesh;
    }

    void MeshComponent::setMesh(const std::string& inMesh)
    {
        m_mesh = Loader::loadMesh(inMesh);
    }

    void MeshComponent::handleDrawability()
    {
        if (!hasActiveCamera())
        {
            return;
        }

        if (getActiveCamera()->canSee(this))
        {
            activateMesh();
        }
        else
        {
            deactivateMesh();
        }
    }

    void MeshComponent::activateMesh()
    {
        if (m_isMeshActive)
        {
            return;
        }

        for (const Box::Entry& entry : m_mesh.entries)
        {
            if (entry.type == Box::EntryType::Model)
            {
                Loader::getModelManager()->activate(entry.reference);
            }
        }

        m_isMeshActive = true;
    }

    void MeshComponent::deactivateMesh()
    {
        if (!m_isMeshActive)
        {
            return;
        }

        for (const Box::Entry& entry : m_mesh.entries)
        {
            if (entry.type == Box::EntryType::Model)
            {
                Loader::getModelManager()->deactivate(entry.reference);
            }
        }

        m_isMeshActive = false;
    }
}