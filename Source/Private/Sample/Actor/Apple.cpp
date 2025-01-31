#include "Actor/Apple.hpp"

#include "Game.hpp"
#include "Chicane/Core.hpp"

Apple::Apple(const Chicane::Vec<3, float>& inStartPosition)
    : Chicane::Actor(),
    m_fallingRate(
        std::max(
            0.001f,
            (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01f
        )
    ),
    m_startPosition(inStartPosition),
    m_mesh(new Chicane::CMesh("Content/Sample/Meshes/Apple.bmsh"))
{
    setCanTick(true);

    setAbsoluteTranslation(inStartPosition);
    setAbsoluteRotation(90.0f, 0.0f, 0.0f);
    setAbsoluteScale(100.0f);

    m_mesh->attachTo(this);
    m_mesh->activate();
}

void Apple::onTick(float inDeltaTime)
{
    if (getAbsoluteTranslation().z <= 0.0f)
    {
        setAbsoluteTranslation(m_startPosition);

        return;
    }

    addAbsoluteRotation(0.0f, 0.0f, 0.1f);
    addAbsoluteTranslation(0.0f, 0.0f, -m_fallingRate);
}

void Apple::onHit(const Chicane::Actor* inSubject)
{
    if (!canTick() || getAbsoluteTranslation().z < 0.0f)
    {
        return;
    }

    setCanTick(false);

    m_mesh->deactivate();

    Game::incrementScore(1);
}