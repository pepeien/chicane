#include "Actor/Structure.hpp"

Strcuture::Strcuture(const std::string& inMesh)
    : Chicane::Actor(),
    m_mesh(new Chicane::CMesh(inMesh))
{
    m_mesh->attachTo(this);
    m_mesh->activate();
}