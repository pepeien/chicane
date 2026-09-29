#pragma once

#include <cstdint>
#include <memory>

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Runtime/Controller.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>
#include <Chicane/Runtime/Scene/Trace/Request.hpp>

#include "Editor/Component/Gizmo/Axis.hpp"
#include "Editor/Component/Gizmo/Type.hpp"

namespace Editor
{
    CH_TYPE(Manual)
    class Gizmo : public Chicane::Component
    {
    public:
        constexpr float HANDLE_SCALE  = 0.10f;
        constexpr float MIN_HANDLE    = 0.25f;
        constexpr float MAX_HANDLE    = 250.0f;
        constexpr float HANDLE_GLOW   = 1.85f;
        constexpr float MIN_SCALE     = 0.01f;
        constexpr float AXIS_START    = 0.22f;
        constexpr float AXIS_END      = 1.55f;
        constexpr float AXIS_PICK     = 0.22f;
        constexpr float AXIS_GAP      = 0.177f;
        constexpr float RING_RADIUS   = 1.00f;
        constexpr float RING_TUBE     = 0.10f;
        constexpr float PLANE_INNER   = 0.16f;
        constexpr float PLANE_OUTER   = 0.42f;
        constexpr float ORIGIN_RADIUS = 0.165f;
        constexpr float ORIGIN_PICK   = 0.10f;
        constexpr float CENTER_PICK   = 0.24f;

    public:
        Gizmo();
        Gizmo(GizmoType inType);
        ~Gizmo() override;

    protected:
        void onLoad() override;
        void onUnload() override;
        void onTick(float inDeltaTime) override;

    public:
        GizmoType getType() const;
        void setType(GizmoType inType);

        Chicane::Object* getTarget() const;
        void setTarget(Chicane::Object* inTarget);

        bool isDragging() const;
        bool isHandleHovered() const;
        bool isHandle(const Chicane::Object* inObject) const;
        bool hitsHandle(const Chicane::SceneTraceRequest& inTrace) const;

    protected:
        bool isViewportHovered() const;
        bool isRelativeSpace() const;

        void onCursor(const Chicane::Vec2& inLocation);
        void onPress();
        void onRelease();

        void syncTransform();
        void syncOrigin();

        void applyMeshes();

        Chicane::CCamera* activeCamera() const;

        void bindController();
        void unbindController();
        void bindInput(Chicane::Controller* inController);

        void setHandleActive(Chicane::CMesh* inMesh, bool inValue);
        void refreshHandleVisibility();
        GizmoAxis axisFromHandle(const Chicane::CMesh* inHandle) const;
        Chicane::CMesh* createHandle(const Chicane::FileSystem::Path& inMesh);
        Chicane::CMesh* hoveredHandle(const Chicane::SceneTraceRequest& inTrace) const;
        void setHoveredHandle(Chicane::CMesh* inHandle);

        Chicane::Vec3 axisDirection(GizmoAxis inAxis) const;
        Chicane::Vec3 planeNormal(GizmoAxis inAxis) const;

        bool cursorTrace(const Chicane::Vec2& inLocation, Chicane::SceneTraceRequest& outTrace) const;

        void beginDrag(const Chicane::SceneTraceRequest& inTrace);
        void drag(const Chicane::SceneTraceRequest& inTrace);
        void endDrag();
        void captureDragStart();

        void poseHandles();

        void applyTranslationDelta(const Chicane::Vec3& inWorldDelta);
        void applyRotationDelta(float inDelta);
        void applyScaleValue(const Chicane::Vec3& inScale);
        float handleScale() const;

    protected:
        bool                  m_bIsDragging;
        std::shared_ptr<bool> m_bIsListening;

        GizmoType m_type;

        Chicane::Object* m_target;
        Chicane::CMesh*  m_origin;
        Chicane::CMesh*  m_x;
        Chicane::CMesh*  m_y;
        Chicane::CMesh*  m_z;
        Chicane::CMesh*  m_xy;
        Chicane::CMesh*  m_xz;
        Chicane::CMesh*  m_yz;
        Chicane::CMesh*  m_center;
        Chicane::CMesh*  m_hovered;

        GizmoAxis        m_dragAxis;
        float            m_dragStartT;
        float            m_dragStartAngle;
        Chicane::Vec3    m_dragOrigin;
        Chicane::Vec3    m_dragAxisDir;
        Chicane::Vec3    m_dragStartHit;
        Chicane::Vec3    m_dragStartTranslation;
        Chicane::Vec3    m_dragStartScale;
        Chicane::Rotator m_dragStartRotation;

        Chicane::Vec2 m_cursor;

        Chicane::Controller::PawnSubscription m_pawnSubscription;
    };
}
