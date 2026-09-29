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

    CH_TYPE(Manual, Group = "Component | Physics")
    class CHICANE_RUNTIME CPhysics : public Component
    {
    public:
        CH_CONSTRUCTOR()
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
        void onPropertyEdited(const String& inName) override;

    public:
        bool hasBody() const;

        void setShape(Kerb::BodyShape inType);
        void setShape(const Kerb::BodyPolygon& inPolygon);

        Kerb::BodyShape getShape() const;
        void appendDebugWireframe(Vertex::List& outVertices, const Vec4& inColor) const;

        void setMotion(Kerb::MotionType inType);

        CH_FUNCTION()
        void setMass(float inMass);

        void setMassScale(float inScale);

        CH_FUNCTION()
        void setGravityFactor(float inFactor);

        CH_FUNCTION()
        float getGravityFactor() const;

        void setObjectLayer(Kerb::ObjectLayer inLayer);
        void setCollisionPreset(Kerb::CollisionPreset inPreset);
        void setCollisionGroup(const Kerb::CollisionGroup& inGroup);
        void setSensor(bool bInSensor);

        CH_FUNCTION()
        void moveTo(const Vec3& inLocation);

        CH_FUNCTION()
        void moveBy(const Vec3& inOffset);

        CH_FUNCTION()
        Vec3 getLinearVelocity() const;

        CH_FUNCTION()
        void setLinearVelocity(const Vec3& inVelocity);

        void setHorizontalVelocity(const Vec3& inVelocity);

        CH_FUNCTION()
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
        void captureSyncedTransform();
        Vec3 colliderCenter() const;
        void applyBodyTransform();
        Transform makeBodyTransform(const Vec3& inLocation) const;

    public:
        CH_FIELD(Group = "Body")
        Kerb::BodyCreateInfo body;

    public:
        Kerb::Body m_body;
        Vec3       m_syncedScale;
        Vec3       m_syncedLocalSize;
        Vec3       m_syncedRelativeTranslation;
        Vec3       m_syncedRelativeRotation;
        Vec3       m_actorToBody;
        bool       m_bIsSyncingBody;
    };
}
