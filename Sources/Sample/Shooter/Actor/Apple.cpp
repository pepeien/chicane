#include "Actor/Apple.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Application.hpp"
#include "Game.hpp"

Apple::Apple()
    : Chicane::Actor(),
      m_rotateRate(std::max(0.1f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.5f)),
      m_fallRate(std::max(0.001f, (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 0.02f)),
      m_startPosition(Chicane::Vec3::Zero),
      m_meshComponent(nullptr),
      m_physicsComponent(nullptr)
{
    setCanTick(true);
    setCanCollide(true);

    m_meshComponent = Chicane::Application::getScene()->createComponent<Chicane::CMesh>();
    m_meshComponent->setMesh("Contents/Sample/Shooter/Meshes/Apple.bmsh");
    m_meshComponent->attachTo(this);
    m_meshComponent->activate();

    m_physicsComponent = Chicane::Application::getScene()->createComponent<Chicane::CPhysics>();
    m_physicsComponent->attachTo(this);
    m_physicsComponent->activate();
}

void Apple::onTick(float inDeltaTime)
{
    addAbsoluteRotation(0.0f, 0.0f, m_rotateRate * inDeltaTime);
    addAbsoluteTranslation(0.0f, 0.0f, -(m_fallRate * inDeltaTime));
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

    Chicane::Application::getScene()->removeComponent(m_meshComponent);

    Game::incrementScore(1);
}

void Apple::setInitialPosition(const Chicane::Vec3& inPosition)
{
    m_startPosition = inPosition;

    setAbsoluteTranslation(m_startPosition);
}