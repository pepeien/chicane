#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Game.hpp"

class Apple : public Chicane::Actor
{
public:
    Apple();

protected:
    void onTick(float inDeltaTime) override;
    void onCollision(const Chicane::Actor* inSubject) override;

public:
    void onHit(const Chicane::Actor* inSubject);

    void setInitialPosition(const Chicane::Vec<3, float>& inPosition);

private:
    float                  m_fallingRate;

    Chicane::Vec<3, float> m_startPosition;

    Chicane::CMesh*        m_meshComponent;
    Chicane::CPhysics*     m_physicsComponent;
};