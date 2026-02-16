#pragma once

#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Component/Physics.hpp>
#include <Chicane/Runtime/Scene/Component/Sound.hpp>

class Apple : public Chicane::Actor
{
public:
    Apple();

protected:
    void onLoad() override;
    void onTick(float inDeltaTime) override;
    void onCollision(const Chicane::Actor* inSubject) override;

public:
    void onHit(const Chicane::Actor* inSubject);

    void setInitialPosition(const Chicane::Vec3& inPosition);

private:
    float m_rotateRate;
    float m_fallRate;

    Chicane::Vec3 m_startPosition;

    Chicane::CMesh*    m_meshComponent;
    Chicane::CPhysics* m_physicsComponent;
    Chicane::CSound*   m_hitSound;
};