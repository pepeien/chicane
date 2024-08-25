#include "Actor/Apple.hpp"

#include "Chicane/Core.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_willGoUp(true),
    m_upTarget(20.0f),
    m_mesh(std::make_unique<Chicane::MeshComponent>())
{
    setCanTick(true);
    setAbsoluteScale(Chicane::Vec<3, float>(0.025f));

    m_mesh->setOwner(this);
    m_mesh->setCanTick(true);
    m_mesh->setMesh("Content/Sample/Meshes/Apple.box");
    m_mesh->activate();
}

void Apple::onTick(float inDeltaTime)
{
    setRelativeRotation(
        Chicane::Vec<3, float>(
            0.0f,
            0.0f,
            0.05f * inDeltaTime
        )
    );
} 