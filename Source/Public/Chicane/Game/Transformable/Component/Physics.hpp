#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Transformable/Component/Instance.hpp"

namespace Chicane
{
    class Actor;

    class CPhysics : public Component
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