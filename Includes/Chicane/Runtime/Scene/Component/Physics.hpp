#pragma once

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    class Actor;

    class CHICANE_RUNTIME CPhysics : public Component
    {
    public:
        CPhysics();

    protected:
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;

        void addForce(const Vec3& inDirection, float inForce);

    protected:
        void updateCollision();
        void resetCollision(Actor* inSubject);

        void updateForce();
        void resetForce();

    protected:
        bool m_bIsApplyingForce;

        Vec3 m_forceDirection;
        Vec3 m_forceVelocity;
    };
}