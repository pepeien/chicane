#include "Actor/Apple.hpp"

#include "Game.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Application.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_fallingRate(
        std::max(
            0.001f,
            (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01f
        )
    ),
    m_startPosition(Chicane::Vec3Zero),
    m_mesh(nullptr)
{
    setCanTick(true);
    setCanCollide(true);
    setAbsoluteTranslation(m_startPosition);
    setAbsoluteRotation(90.0f, 0.0f, 0.0f);
    setAbsoluteScale(100.0f);

    m_mesh = Chicane::Application::getLevel()->createComponent<Chicane::CMesh>();
    m_mesh->setMesh("Content/Sample/Meshes/Apple.bmsh");
    m_mesh->attachTo(this);
    m_mesh->activate();
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

    setCanTick(true);

    m_mesh->deactivate();

    Game::incrementScore(1);
}

void Apple::setInitialPosition(const Chicane::Vec<3, float>& inPosition)
{
    m_startPosition = inPosition;
}