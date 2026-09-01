#include "Sample/Shooter/Actor/Apple.hpp"

#include <Chicane/Runtime/Scene.hpp>

#include "Sample/Shooter/Game.hpp"

Apple::Apple()
    : Chicane::Actor(),
      m_rotateRate(std::max(0.1f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.5f)),
      m_fallRate(std::max(0.001f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.02f)),
      m_startPosition(Chicane::Vec3::Zero()),
      m_meshComponent(nullptr),
      m_hitSound(nullptr)
{
    setCanTick(true);
    setCanCollide(true);
}

void Apple::onLoad()
{
    static const Chicane::FileSystem::Path meshPath("Assets/Sample/Shooter/Meshes/Apple.bmsh");

    m_meshComponent = getScene()->createComponent<Chicane::CMesh>();
    m_meshComponent->setMesh(meshPath);
    m_meshComponent->attachTo(this);
    m_meshComponent->activate();
}

void Apple::onTick(float inDeltaTime)
{
    if (!m_meshComponent)
    {
        return;
    }

    m_meshComponent->addRelativeRotation(0.0f, 0.0f, m_rotateRate);
    //addAbsoluteTranslation(0.0f, 0.0f, -m_fallRate);
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

    m_meshComponent = nullptr;

    Game::incrementScore(1);

    if (!m_hitSound)
    {
        m_hitSound = getScene()->createComponent<Chicane::CSound>();
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