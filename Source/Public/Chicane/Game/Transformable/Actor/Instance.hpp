#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Game/Transformable/Instance.hpp"

namespace Chicane
{
    class Actor : public Transformable
    {
    public:
        Actor();
        virtual ~Actor() = default;

    protected:
        virtual void onTick(float inDeltaTime) { return; };
        virtual void onCollision(const Actor* inSubject);

    public:
        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        bool canCollide() const;
        void setCanCollide(bool inCanCollide);
        bool isCollidingWith(const Actor* inSubject) const;
        bool isCollidingWith(const Bounds& inBounds) const;
        bool isCollidingWith(const Vec<3, float>& inPoint) const;
        void collideWith(const Actor* inSubject);

        void addForce(const Vec<3, float>& inDirection, float inForce);

    protected:
        void processCollision();

        void processForce();
        void resetForce();

    protected:
        bool          m_bCanTick;
        bool          m_bCanCollide;
        bool          m_bIsRigidBodyEnabled;
        bool          m_bIsApplyingForce;

        Vec<3, float> m_forceDirection;
        Vec<3, float> m_forceVelocity;
    };
} 