#include "Actor/Structure.hpp"

#include <Chicane/Runtime/Scene.hpp>

Strcuture::Strcuture()
    : Chicane::Actor(),
      m_mesh(nullptr)
{
    setCanCollide(true);
}

void Strcuture::onLoad()
{
    m_mesh = getScene()->createComponent<Chicane::CMesh>();
    m_mesh->attachTo(this);
    m_mesh->setMesh("Contents/Sample/Shooter/Meshes/Structure.bmsh");
    m_mesh->activate();
}