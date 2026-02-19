#pragma once

#include "Chicane/Core/Math/Bounds/3D.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Object.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME Actor : public Object
    {
    public:
        Actor();

    protected:
        inline virtual void onCollision(const Actor* inSubject) { return; }

    public:
        bool canCollide() const;
        void setCanCollide(bool inCanCollide);
        bool isCollidingWith(const Actor* inSubject) const;
        bool isCollidingWith(const Bounds3D& inBounds) const;
        bool isCollidingWith(const Vec3& inPoint) const;
        void collideWith(const Actor* inSubject);

    protected:
        bool m_bCanCollide;
    };
}