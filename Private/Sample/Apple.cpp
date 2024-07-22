#include "Apple.hpp"

#include "Chicane/Core.hpp"

Apple::Apple()
    : Chicane::Actor()
{
    setMesh(Chicane::Box::read("Content/Sample/Meshes/Apple.box"));
    setCanTick(true);
}

void Apple::onTick(float inDeltaTime)
{
    setRelativeRotation(glm::vec3(0.0f, 0.0f, 0.05f));
} 