#include "Chicane/Game/Actor/Component/Mesh.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    MeshComponent::MeshComponent()
        : ActorComponent()
    {}

    void MeshComponent::onActivation()
    {
        if (!isDrawable())
        {
            return;
        }

        for (const Box::Entry& entry : m_mesh.entries)
        {
            if (entry.type == Box::EntryType::Model)
            {
                Loader::getModelManager()->use(entry.reference);
            }
        }

        getActiveLevel()->addMesh(this);
    }

    void MeshComponent::onDeactivation()
    {
        if (!hasMesh() || !hasActiveLevel())
        {
            return;
        }

        for (const Box::Entry& entry : m_mesh.entries)
        {
            if (entry.type == Box::EntryType::Model)
            {
                Loader::getModelManager()->unUse(entry.reference);
            }
        }
    }

    bool MeshComponent::isDrawable() const
    {
        return hasMesh() && hasActiveLevel() && isActive();
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
}