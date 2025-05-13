#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Runtime/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class Actor;

    class CHICANE CPhysics : public Component
    {
    public:
        CPhysics();
        virtual ~CPhysics() = default;

    protected:
        void onTick(float inDeltaTime) override;

    public:
        bool canCollide() const;

        void addForce(const Vec<3, float>& inDirection, float inForce);

    protected:
        void updateCollision();
        void resetCollision(Actor* inSubject);

        void updateForce();
        void resetForce();

    protected:
        bool          m_bIsApplyingForce;

        Vec<3, float> m_forceDirection;
        Vec<3, float> m_forceVelocity;
    };
}