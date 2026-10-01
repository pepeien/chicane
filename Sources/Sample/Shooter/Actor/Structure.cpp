#include "Sample/Shooter/Actor/Structure.reflected.hpp"

#include <Chicane/Runtime/Scene.hpp>

Strcuture::Strcuture()
    : Chicane::Actor(),
      m_mesh(nullptr),
      m_physics(nullptr)
{
    setCanCollide(true);
}

void Strcuture::createDefaultComponents()
{
    m_mesh = createDefaultComponent<Chicane::CMesh>("Mesh");
    if (m_mesh)
    {
        m_mesh->setMesh("Assets/Sample/Shooter/Meshes/Structure.bmsh");
        m_mesh->activate();
    }

    m_physics = createDefaultComponent<Chicane::CPhysics>("Physics");
    if (!m_physics)
    {
        return;
    }

    m_physics->setShape(Chicane::Kerb::BodyShape::Box);
    m_physics->setMotion(Chicane::Kerb::MotionType::Static);
    m_physics->setCollisionPreset(Chicane::Kerb::CollisionPreset::BlockAll);
    m_physics->activate();
}
