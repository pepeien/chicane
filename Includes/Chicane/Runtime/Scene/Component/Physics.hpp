#pragma once

#include "Chicane/Core/Math/Transform.hpp"
#include "Chicane/Core/Math/Vec/Vec4.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/Reflection.hpp"

#include "Chicane/Kerb/Body.hpp"
#include "Chicane/Kerb/Body/CreateInfo.hpp"
#include "Chicane/Kerb/Body/Shape.hpp"
#include "Chicane/Kerb/Collision/Group.hpp"
#include "Chicane/Kerb/Collision/Preset.hpp"
#include "Chicane/Kerb/Motion/Type.hpp"
#include "Chicane/Kerb/Object/Layer.hpp"

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
        bool hasBody() const;

        void setShape(Kerb::BodyShape inType);
        void setShape(const Kerb::BodyPolygon& inPolygon);

        Kerb::BodyShape getShape() const;
        void appendDebugWireframe(Vertex::List& outVertices, const Vec4& inColor) const;

        void setMotion(Kerb::MotionType inType);

        void setMass(float inMass);
        void setMassScale(float inScale);

        void setGravityFactor(float inFactor);
        float getGravityFactor() const;

        void setObjectLayer(Kerb::ObjectLayer inLayer);
        void setCollisionPreset(Kerb::CollisionPreset inPreset);
        void setCollisionGroup(const Kerb::CollisionGroup& inGroup);
        void setSensor(bool bInSensor);

        void moveTo(const Vec3& inLocation);
        void moveBy(const Vec3& inOffset);

        Vec3 getLinearVelocity() const;
        void setLinearVelocity(const Vec3& inVelocity);
        void setHorizontalVelocity(const Vec3& inVelocity);

        void addImpulse(const Vec3& inDirection, float inForce, const Vec3& inLocation);

    protected:
        bool canCollide() const;

        void ensureBody();
        void destroyBody();
        void rebuildBody();
        void syncBody();
        void syncTickState();
        void syncCollisionSettings();
        void updateCollision();
        Transform makeActorTransform(const Vec3& inLocation) const;

    protected:
        Kerb::Body           m_body;
        Kerb::BodyCreateInfo m_bodySettings;
        Vec3                 m_syncedScale;
        Vec3                 m_syncedLocalSize;
        Vec3                 m_actorToBody;
        bool                 m_bSyncingBody;
    };
}
