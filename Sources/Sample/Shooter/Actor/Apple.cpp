#include "Sample/Shooter/Actor/Apple.hpp"

#include <algorithm>

#include <Chicane/Kerb/Body/Shape.hpp>
#include <Chicane/Kerb/Collision/Preset.hpp>
#include <Chicane/Kerb/Gravity.hpp>
#include <Chicane/Kerb/Motion/Type.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Sample/Shooter/Actor/Structure.hpp"
#include "Sample/Shooter/Game.hpp"

Apple::Apple()
    : Chicane::Actor(),
      m_rotateRate(std::max(0.1f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.5f)),
      m_fallRate(std::max(0.001f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.02f)),
      m_startPosition(Chicane::Vec3::sZero()),
      m_meshComponent(nullptr),
      m_physics(nullptr),
      m_hitSound(nullptr)
{
    setCanTick(true);
    setCanCollide(true);
}

void Apple::onLoad()
{
    m_meshComponent = getScene()->createComponent<Chicane::CMesh>();
    m_meshComponent->setOrigin(Chicane::ObjectOrigin::Native);
    m_meshComponent->setId("Mesh");
    m_meshComponent->attachTo(this);
    m_meshComponent->setMesh("Assets/Sample/Shooter/Meshes/Apple.bmsh");
    m_meshComponent->activate();

    m_physics = getScene()->createComponent<Chicane::CPhysics>();
    m_physics->setOrigin(Chicane::ObjectOrigin::Native);
    m_physics->setId("Physics");
    m_physics->attachTo(this);
    m_physics->setShape(Chicane::Kerb::BodyShape::Box);
    m_physics->setMotion(Chicane::Kerb::MotionType::Kinematic);
    m_physics->setCollisionPreset(Chicane::Kerb::CollisionPreset::Custom_0);
    m_physics->setGravityFactor(Chicane::Kerb::Gravity::FactorNone);
    m_physics->activate();
}

void Apple::onTick(float inDeltaTime)
{
    if (!m_meshComponent)
    {
        return;
    }

    addRelativeRotation(0.0f, 0.0f, m_rotateRate);
    addAbsoluteTranslation(0.0f, 0.0f, -m_fallRate);
}

void Apple::onCollision(const Chicane::Actor* inSubject)
{
    if (!canTick() || !dynamic_cast<const Strcuture*>(inSubject))
    {
        return;
    }

    setAbsoluteTranslation(m_startPosition);
}

void Apple::onHit(const Chicane::Actor* inSubject)
{
    if (!canTick() || getAbsoluteTranslation().z < 0.0f)
    {
        return;
    }

    setCanTick(false);

    if (m_physics)
    {
        m_physics->deactivate();
    }

    m_meshComponent->deactivate();

    getScene()->removeComponent(m_meshComponent);

    m_meshComponent = nullptr;

    Game::incrementScore(1);

    if (!m_hitSound)
    {
        m_hitSound = getScene()->createComponent<Chicane::CSound>();
        m_hitSound->setOrigin(Chicane::ObjectOrigin::Spawned);
        m_hitSound->load("Assets/Sample/Shooter/Sounds/Hit.bsnd");
        m_hitSound->attachTo(this);
        m_hitSound->activate();
    }

    m_hitSound->play();
}

void Apple::setInitialPosition(const Chicane::Vec3& inPosition)
{
    m_startPosition = inPosition;

    setAbsoluteTranslation(m_startPosition);
}