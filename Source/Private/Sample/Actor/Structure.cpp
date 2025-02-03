#include "Actor/Structure.hpp"

#include "Chicane/Application.hpp"

Strcuture::Strcuture()
    : Chicane::Actor(),
    m_mesh(nullptr)
{
    setCanCollide(true);

    m_mesh = Chicane::Application::getLevel()->createComponent<Chicane::CMesh>();
    m_mesh->attachTo(this);
    m_mesh->setMesh("Content/Sample/Meshes/Cube.bmsh");
    m_mesh->activate();
}