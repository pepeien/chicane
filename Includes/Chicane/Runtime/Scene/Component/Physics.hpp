#pragma once

#include "Chicane/Kerb/Body/Shape.hpp"
#include "Chicane/Kerb/Engine.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"

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
        void onLoad() override;
        void onTick(float inDeltaTime) override;
        void onActivation() override;
        void onDeactivation() override;

    public:
        void setShape(Kerb::BodyShape inType);
        void setShape(const Kerb::BodyPolygon& inPolygon);

        void setMotion(Kerb::MotionType inType);

        void moveTo(const Vec3& inLocation);

        void addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation);

    protected:
        bool canCollide() const;
        bool hasBody() const;

    protected:
        JPH::BodyID          m_bodyID;
        Kerb::BodyCreateInfo m_bodySettings;
    };
}