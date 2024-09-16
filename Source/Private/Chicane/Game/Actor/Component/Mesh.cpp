#include "Chicane/Game/Actor/Component/Mesh.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game/Actor/Component/Camera.hpp"

namespace Chicane
{
    MeshComponent::MeshComponent()
        : ActorComponent(),
        m_bIsMeshActive(false)
    {}

    void MeshComponent::onActivation()
    {
        if (!hasActiveLevel())
        {
            return;
        }

        activateMesh();

        addComponent(this);
    }

    void MeshComponent::onDeactivation()
    {
        if (!hasActiveLevel())
        {
            return;
        }

        deactivateMesh();
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
        //handleDrawability();
    }

    bool MeshComponent::isDrawable() const
    {
        if (!hasActiveCamera() || !hasActiveLevel())
        {
            return false;
        }

        return hasMesh() && isActive() && m_bIsMeshActive;
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

        Bounds bounds {};

        for (const Box::Entry& entry : m_mesh.entries)
        {
            if (entry.type != Box::EntryType::Model)
            {
                continue;
            }

            const Vec<3, float>& extent = Loader::getModelManager()->getBounds(entry.reference);

            bounds.extent.x = std::max(
                bounds.extent.x,
                extent.x
            );
            bounds.extent.y = std::max(
                bounds.extent.y,
                extent.y
            );
            bounds.extent.z = std::max(
                bounds.extent.z,
                extent.z
            );
        }

        setBounds(bounds);

        if (!isAttached())
        {
            return;
        }

        m_attachment->setBounds(bounds);
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
        if (m_bIsMeshActive)
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

        m_bIsMeshActive = true;
    }

    void MeshComponent::deactivateMesh()
    {
        if (!m_bIsMeshActive)
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

        m_bIsMeshActive = false;
    }
}