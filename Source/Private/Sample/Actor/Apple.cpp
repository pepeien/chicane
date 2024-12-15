#include "Actor/Apple.hpp"

#include "Game.hpp"
#include "Chicane/Core.hpp"

Apple::Apple(
    const std::string& inId,
    const Chicane::Vec<3, float>& inStartPosition
)
    : Chicane::Actor(),
    m_fallingRate(
        std::max(
            0.001f,
            (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.01f
        )
    ),
    m_startPosition(inStartPosition),
    m_mesh(new Chicane::MeshComponent())
{
    setCanTick(true);
    setCanCollide(true);
    setAbsoluteTranslation(inStartPosition);
    setAbsoluteScale(Chicane::Vec<3, float>(0.05f));

    m_mesh->attachTo(this);
    m_mesh->setMesh(inId);
    m_mesh->activate();
}

void Apple::onTick(float inDeltaTime)
{
    if (m_transform.translation.z <= 0.0f)
    {
        setAbsoluteTranslation(m_startPosition);

        return;
    }

    setRelativeRotation(
        Chicane::Vec<3, float>(
            0.0f,
            0.0f,
            0.1f
        )
    );
    setRelativeTranslation(
        Chicane::Vec<3, float>(
            0.0f,
            0.0f,
            -m_fallingRate
        )
    );
}

void Apple::onCollision(const Chicane::Actor* inSubject)
{
    if (!canTick() || m_transform.translation.z < 0.0f)
    {
        return;
    }

    setCanTick(false);

    m_mesh->deactivate();

    Game::incrementScore(1);
}