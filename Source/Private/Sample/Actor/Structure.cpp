#include "Actor/Structure.hpp"

Strcuture::Strcuture(const std::string& inMesh)
    : Chicane::Actor(),
    m_mesh(new Chicane::MeshComponent())
{
    m_mesh->attachTo(this);
    m_mesh->setMesh(inMesh);
    m_mesh->activate();
}