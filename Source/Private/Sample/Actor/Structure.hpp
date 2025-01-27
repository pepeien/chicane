#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Transformable/Component/Mesh.hpp"

class Strcuture : public Chicane::Actor
{
public:
    Strcuture(const std::string& inMesh);

private:
    Chicane::CMesh* m_mesh;
};