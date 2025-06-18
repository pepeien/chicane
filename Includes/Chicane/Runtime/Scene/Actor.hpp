#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Scene/Transformable.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Actor : public Transformable
    {
    protected:
        using Super = Actor;

    public:
        Actor();
        virtual ~Actor() = default;

    protected:
        virtual void onTick(float inDeltaTime) { return; }
        virtual void onCollision(const Actor* inSubject) { return; }

    public:
        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

        bool canCollide() const;
        void setCanCollide(bool inCanCollide);
        bool isCollidingWith(const Actor* inSubject) const;
        bool isCollidingWith(const Bounds& inBounds) const;
        bool isCollidingWith(const Vec3& inPoint) const;
        void collideWith(const Actor* inSubject);

    protected:
        bool m_bCanTick;
        bool m_bCanCollide;
    };
} 