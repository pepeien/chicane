#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

class Strcuture : public Chicane::Actor
{
public:
    Strcuture();

private:
    Chicane::MeshComponent* m_mesh;
};