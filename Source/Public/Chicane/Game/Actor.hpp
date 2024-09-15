#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Game/Transformable.hpp"

namespace Chicane
{
    class Actor : public Transformable
    {
    public:
        Actor();
        virtual ~Actor() = default;

    public:
        virtual void onTick(float inDeltaTime) { return; };
        virtual void onCollision(const Actor* inSubject) { return; };

    public:
        bool canCollide() const;
        void setCanCollide(bool inCanCollide);

        bool isCollidingWith(const Actor* inSubject) const;
        bool isCollidingWith(const Bounds& inBounds) const;
        bool isCollidingWith(const Vec<3, float>& inPoint) const;
        void collideWith(const Actor* inSubject);

        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

    protected:
        bool m_bCanCollide;
        bool m_bCanTick;
    };
} 