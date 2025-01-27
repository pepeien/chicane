#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple(
        const std::string& inMesh,
        const Chicane::Vec<3, float>& inStartPosition
    );

protected:
    void onTick(float inDeltaTime) override;

public:
    void onHit(const Chicane::Actor* inSubject);

private:
    float                  m_fallingRate;

    Chicane::Vec<3, float> m_startPosition;

    Chicane::CMesh*        m_mesh;
};