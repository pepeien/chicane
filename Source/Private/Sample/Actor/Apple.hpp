#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple();

protected:
    void onTick(float inDeltaTime) override;

private:
    Chicane::MeshComponent* m_mesh;
};