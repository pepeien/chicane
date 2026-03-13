#include "Sample/Shooter/Actor/Structure.hpp"

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
    m_mesh = getScene()->createComponent<Chicane::CMesh>();
    m_mesh->attachTo(this);
    m_mesh->setMesh("Assets/Sample/Shooter/Meshes/Structure.bmsh");
    m_mesh->activate();

    m_physics = getScene()->createComponent<Chicane::CPhysics>();
    m_physics->setShape(Chicane::Kerb::BodyShape::Box);
    m_physics->setMotion(Chicane::Kerb::MotionType::Static);
    m_physics->attachTo(this);
    m_physics->activate();
}