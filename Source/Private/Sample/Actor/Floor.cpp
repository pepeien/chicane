#include "Actor/Floor.hpp"

#include "Chicane/Core.hpp"

Floor::Floor()
    : Chicane::Actor()
{
    setRelativeScale(Chicane::Vec<float>::Three(1.5f));

    m_mesh = Chicane::Allocator::load("Content/Sample/Meshes/Cube.box");
}