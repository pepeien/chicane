#include "Actor/Floor.hpp"

Floor::Floor()
    : Chicane::Actor()
{
    setMesh(Chicane::Box::readHeader("Content/Sample/Meshes/Cube.box"));
    setAbsoluteScale(Chicane::Vec<float>::Three(2500.0f, 2500.0f, 10.0f));
}