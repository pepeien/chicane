#pragma once

#include "Chicane/Kerb/Engine.hpp"

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
        void onAttachment(Object* inAttachment) override;

    public:
        bool canCollide() const;

        void moveTo(const Vec3& inLocation);

        void addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation);

        std::pair<Vertex::Indices, Vertex::List> getBodyPolygon() const;

    public:
        bool bisDynamic = true;

    protected:
        JPH::BodyID m_bodyID;
    };
}