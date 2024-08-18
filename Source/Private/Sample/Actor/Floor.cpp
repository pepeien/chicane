#include "Actor/Floor.hpp"

#include "Chicane/Core.hpp"

Floor::Floor()
    : Chicane::Actor()
{
    setAbsoluteScale(Chicane::Vec<float>::Three(2500.0f, 2500.0f, 10.0f));

    m_mesh = Chicane::Allocator::load("Content/Sample/Meshes/Cube.box");
}