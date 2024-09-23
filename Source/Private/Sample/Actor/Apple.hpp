#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple();

protected:
    void onTick(float inDeltaTime);
    void onCollision(const Chicane::Actor* inSubject);

private:
    Chicane::MeshComponent* m_mesh;
};