#include "Actor/Structure.hpp"

#include <Chicane/Runtime/Scene.hpp>

Strcuture::Strcuture()
    : Chicane::Actor(),
      m_mesh(nullptr),
      m_physics(nullptr)
{
    setCanCollide(true);
}

void Strcuture::onLoad()
{
    setAbsoluteScale(500.0f, 500.0f, 0.5);

    m_mesh = getScene()->createComponent<Chicane::CMesh>();
    m_mesh->attachTo(this);
    m_mesh->setMesh("Contents/Sample/Shooter/Meshes/Structure.bmsh");
    m_mesh->activate();

    m_physics             = getScene()->createComponent<Chicane::CPhysics>();
    m_physics->bisDynamic = false;
    m_physics->attachTo(this);
    m_physics->activate();
}