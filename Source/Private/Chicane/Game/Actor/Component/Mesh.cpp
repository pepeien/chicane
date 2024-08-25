#include "Chicane/Game/Actor/Component/Mesh.hpp"

#include "Chicane/Core.hpp"

namespace Chicane
{
    MeshComponent::MeshComponent()
        : ActorComponent()
    {}

    void MeshComponent::onActivation()
    {
        if (!hasMesh() || !hasOwner() || !hasActiveLevel())
        {
            return;
        }

        Allocator::load(m_mesh.filepath);

        getActiveLevel()->addMesh(this);
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
        Box::Instance mesh = Box::readHeader(inMesh);

        if (mesh.type != Box::Type::Mesh)
        {
            LOG_WARNING("The asset [" + inMesh + "] is not a mesh");

            return;
        }

        m_mesh = mesh;
    }
}