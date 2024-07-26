#include "Actor/Floor.hpp"

#include "Chicane/Core.hpp"

Floor::Floor()
    : Chicane::Actor()
{
    Chicane::Allocator::load("Content/Sample/Meshes/Cube.box");

    setAbsoluteScale(Chicane::Vec<float>::Three(2500.0f, 2500.0f, 10.0f));
}