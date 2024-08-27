#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple();

public:
    void onTick(float inDeltaTime) override;

private:
    std::unique_ptr<Chicane::MeshComponent> m_mesh;
};