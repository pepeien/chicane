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
        virtual void onCollision(Actor* inActor) { return; };

    public:
        bool canCollide() const;
        void setCanCollide(bool inCanCollide);
        void collide(Actor* inSubject);

        bool canTick() const;
        void setCanTick(bool inCanTick);
        void tick(float inDeltaTime);

    protected:
        bool m_canCollide;
        bool m_canTick;
    };
} 