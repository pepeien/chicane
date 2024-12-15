#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple(
        const std::string& inId,
        const Chicane::Vec<3, float>& inStartPosition
    );

protected:
    void onTick(float inDeltaTime);
    void onCollision(const Chicane::Actor* inSubject);

private:
    float m_fallingRate;

    Chicane::Vec<3, float> m_startPosition;

    Chicane::MeshComponent* m_mesh;
};