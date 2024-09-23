#include "Actor/Apple.hpp"

#include "Chicane/Core.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_mesh(new Chicane::MeshComponent())
{
    setCanTick(true);
    setCanCollide(true);
    setAbsoluteScale(Chicane::Vec<3, float>(0.05f));

    m_mesh->attachTo(this);
    m_mesh->setMesh("Content/Sample/Meshes/Apple.box");
    m_mesh->activate();
}

void Apple::onTick(float inDeltaTime)
{
    setRelativeRotation(
        Chicane::Vec<3, float>(
            0.0f,
            0.0f,
            0.1f
        )
    );
}

void Apple::onCollision(const Chicane::Actor* inSubject)
{
    setCanTick(!canTick());
}