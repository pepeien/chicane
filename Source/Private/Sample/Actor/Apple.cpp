#include "Actor/Apple.hpp"

#include "Chicane/Core.hpp"

Apple::Apple()
    : Chicane::Actor(),
    m_willGoUp(true),
    m_upTarget(20.0f)
{
    setCanTick(true);

    setMesh(Chicane::Box::readHeader("Content/Sample/Meshes/Apple.box"));
}

void Apple::onTick(float inDeltaTime)
{
    //setRelativeTranslation(Chicane::Vec<float>::Three(0.0f, 0.0f, 0.05f * inDeltaTime));
    setRelativeRotation(Chicane::Vec<float>::Three(0.0f, 0.0f, 0.05f * inDeltaTime));
} 