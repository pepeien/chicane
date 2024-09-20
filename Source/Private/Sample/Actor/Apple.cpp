#include "Actor/Apple.hpp"

#include "Chicane/Core.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_mesh(new Chicane::MeshComponent())
{
    setCanCollide(true);

    m_mesh->attachTo(this);
    m_mesh->setMesh("Content/Sample/Meshes/Cube.box");
    m_mesh->activate();
}