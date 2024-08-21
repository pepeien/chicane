#include "Actor/Apple.hpp"

#include "Chicane/Core.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_willGoUp(true),
    m_upTarget(20.0f)
{
    setCanTick(true);
    setAbsoluteScale(Chicane::Vec<float>::Three(0.025f));

    m_mesh = Chicane::Allocator::load("Content/Sample/Meshes/Apple.box");
}

void Apple::onTick(float inDeltaTime)
{
    setRelativeRotation(
        Chicane::Vec<float>::Three(
            0.0f,
            0.0f,
            0.05f * inDeltaTime
        )
    );
} 