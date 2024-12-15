#include "Actor/Structure.hpp"

Strcuture::Strcuture()
    : Chicane::Actor(),
    m_mesh(new Chicane::MeshComponent())
{
    m_mesh->attachTo(this);
    m_mesh->setMesh("Content/Sample/Meshes/Cube.bmsh");
    m_mesh->activate();
}