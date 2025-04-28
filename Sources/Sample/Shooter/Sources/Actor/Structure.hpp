#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Game.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Mesh.hpp"

class Strcuture : public Chicane::Actor
{
public:
    Strcuture();

private:
    Chicane::CMesh* m_mesh;
};