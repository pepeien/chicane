#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Transformable/Component/MeshComponent.hpp"

class Strcuture : public Chicane::Actor
{
public:
    Strcuture();

private:
    Chicane::MeshComponent* m_mesh;
};