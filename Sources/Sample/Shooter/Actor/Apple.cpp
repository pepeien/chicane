#include "Actor/Apple.hpp"

#include <Chicane/Runtime/Scene.hpp>

#include "Game.hpp"

Apple::Apple()
    : Chicane::Actor(),
      m_rotateRate(std::max(0.1f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.5f)),
      m_fallRate(std::max(0.001f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.02f)),
      m_startPosition(Chicane::Vec3::Zero),
      m_meshComponent(nullptr),
      m_physicsComponent(nullptr),
      m_hitSound(nullptr)
{
    setCanTick(true);
    setCanCollide(true);
}

void Apple::onLoad()
{
    m_meshComponent = getScene()->createComponent<Chicane::CMesh>();
    m_meshComponent->setMesh("Contents/Sample/Shooter/Meshes/Apple.bmsh");
    m_meshComponent->attachTo(this);
    m_meshComponent->activate();

    m_physicsComponent = getScene()->createComponent<Chicane::CPhysics>();
    m_physicsComponent->attachTo(this);
    m_physicsComponent->activate();

    m_hitSound = getScene()->createComponent<Chicane::CSound>();
    m_hitSound->load("Contents/Sample/Shooter/Sounds/Hit.bsnd");
    m_hitSound->attachTo(this);
    m_hitSound->activate();
}

void Apple::onTick(float inDeltaTime)
{
    addAbsoluteRotation(0.0f, 0.0f, m_rotateRate);
    addAbsoluteTranslation(0.0f, 0.0f, -m_fallRate);
}

void Apple::onCollision(const Chicane::Actor* inSubject)
{
    setAbsoluteTranslation(m_startPosition);
}

void Apple::onHit(const Chicane::Actor* inSubject)
{
    if (!canTick() || getAbsoluteTranslation().z < 0.0f)
    {
        return;
    }

    setCanTick(false);

    m_meshComponent->deactivate();

    getScene()->removeComponent(m_meshComponent);

    Game::incrementScore(1);

    m_hitSound->play();
}

void Apple::setInitialPosition(const Chicane::Vec3& inPosition)
{
    m_startPosition = inPosition;

    setAbsoluteTranslation(m_startPosition);
}