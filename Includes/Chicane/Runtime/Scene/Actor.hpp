#pragma once

#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Transformable.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Actor : public Transformable
    {
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
        bool isCollidingWith(const Bounds3D& inBounds) const;
        bool isCollidingWith(const Vec3& inPoint) const;
        void collideWith(const Actor* inSubject);

        const String& getId() const;
        void setId(const String& inId);

    protected:
        bool   m_bCanTick;
        bool   m_bCanCollide;

        String m_id;
    };
}