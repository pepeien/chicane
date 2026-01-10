#include "Actor/Structure.hpp"

#include "Chicane/Runtime/Application.hpp"

Strcuture::Strcuture()
    : Chicane::Actor(),
      m_mesh(nullptr)
{
    setCanCollide(true);

    m_mesh = Chicane::Application::getScene()->createComponent<Chicane::CMesh>();
    m_mesh->attachTo(this);
    m_mesh->setMesh("Contents/Sample/Shooter/Meshes/Structure.bmsh");
    m_mesh->activate();
}