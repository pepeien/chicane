#pragma once

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Kerb/Body.hpp"
#include "Chicane/Kerb/Body/CreateInfo.hpp"
#include "Chicane/Kerb/Body/Shape.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"

#include "Chicane/Runtime.hpp"
#include "Chicane/Runtime/Scene/Component.hpp"

namespace Chicane
{
    class Actor;

    CH_TYPE(Manual)
    class CHICANE_RUNTIME CPhysics : public Component
    {
    public:
        CPhysics();
        ~CPhysics() override;

    protected:
        void onLoad() override;
        void onUnload() override;
        void onTick(float inDeltaTime) override;
        void onActivation() override;
        void onDeactivation() override;
        void onAttachment(Object* inParent) override;
        void onRefresh() override;

    public:
        void setShape(Kerb::BodyShape inType);
        void setShape(const Kerb::BodyPolygon& inPolygon);

        void setMotion(Kerb::MotionType inType);

        void moveTo(const Vec3& inLocation);
        void moveBy(const Vec3& inOffset);

        Vec3 getLinearVelocity() const;
        void setLinearVelocity(const Vec3& inVelocity);
        void setHorizontalVelocity(const Vec3& inVelocity);

        void addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation);

    protected:
        bool canCollide() const;
        bool hasBody() const;

        void ensureBody();
        void destroyBody();
        void syncBody();
        void syncTickState();
        Transform makeActorTransform(const Vec3& inLocation) const;

    protected:
        Kerb::Body           m_body;
        Kerb::BodyCreateInfo m_bodySettings;
        Vec3                 m_syncedScale;
        Vec3                 m_syncedLocalSize;
        bool                 m_bSyncingBody;
    };
}
