#include "Actor/Apple.hpp"

#include "Game.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Application.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_fallingRate(
        std::max(
            0.001f,
            (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01f
        )
    ),
    m_startPosition(Chicane::Vec3Zero),
    m_meshComponent(nullptr),
    m_physicsComponent(nullptr)
{
    setCanTick(true);
    setCanCollide(true);
    setAbsoluteScale(1.5f);

    m_meshComponent = Chicane::Application::getLevel()->createComponent<Chicane::CMesh>();
    m_meshComponent->setMesh("Contents/Shooter/Meshes/Apple.bmsh");
    m_meshComponent->attachTo(this);
    m_meshComponent->activate();

    m_physicsComponent = Chicane::Application::getLevel()->createComponent<Chicane::CPhysics>();
    m_physicsComponent->attachTo(this);
    m_physicsComponent->activate();
}

void Apple::onTick(float inDeltaTime)
{
    addAbsoluteRotation(0.0f, 0.0f, 0.1f);
    addAbsoluteTranslation(0.0f, 0.0f, -m_fallingRate);
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

    Game::incrementScore(1);
}

void Apple::setInitialPosition(const Chicane::Vec<3, float>& inPosition)
{
    m_startPosition = inPosition;

    setAbsoluteTranslation(m_startPosition);
}