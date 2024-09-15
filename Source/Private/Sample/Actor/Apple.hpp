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
    void onCollision(const Chicane::Actor* inActor) override;

public:
    void respawn();
    void die();

private:
    bool m_bIsDead;

    Chicane::MeshComponent* m_mesh;
};