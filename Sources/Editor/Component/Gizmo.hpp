#pragma once

#include <memory>

#include <Chicane/Core/Event/Subscription.hpp>
#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Vec/Vec2.hpp>
#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Runtime/Scene/Component.hpp>
#include <Chicane/Runtime/Scene/Component/Mesh.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/Component/Gizmo/Axis.hpp"
#include "Editor/Component/Gizmo/Type.hpp"

namespace Editor
{
    CH_TYPE(Manual)
    class Gizmo : public Chicane::Component
    {
    public:
        Gizmo();
        Gizmo(GizmoType inType);
        ~Gizmo() override;

    public:
        GizmoType getType() const;
        void setType(GizmoType inType);

        Chicane::Object* getTarget() const;
        void setTarget(Chicane::Object* inTarget);

        bool isDragging() const;
        bool pick(const Chicane::Vec3& inOrigin, const Chicane::Vec3& inDirection);
        void drag(const Chicane::Vec3& inOrigin, const Chicane::Vec3& inDirection);
        void endDrag();

    protected:
        void onLoad() override;
        void onUnload() override;
        void onActivation() override;
        void onDeactivation() override;

        void applyMesh();
        void syncTransform();
        Chicane::Vec3 axisDirection(GizmoAxis inAxis) const;

        void bindWindow();
        void unbindWindow();
        void onWindowEvent(const Chicane::WindowEvent& inEvent);
        bool makeRay(const Chicane::Vec2& inLocation, Chicane::Vec3& outOrigin, Chicane::Vec3& outDirection) const;
        bool isOverViewport(const Chicane::Vec2& inLocation) const;
        void pickAt(const Chicane::Vec2& inLocation);
        void dragAt(const Chicane::Vec2& inLocation);

    protected:
        GizmoType                            m_type;
        Chicane::CMesh*                      m_mesh;
        Chicane::Object*                     m_target;
        Chicane::EventSubscription<>         m_targetSubscription;

        bool                                 m_bIsDragging;
        GizmoAxis                            m_dragAxis;
        float                                m_dragStartT;
        float                                m_dragStartAngle;
        Chicane::Vec3                        m_dragOrigin;
        Chicane::Vec3                        m_dragAxisDir;
        Chicane::Vec3                        m_dragStartTranslation;
        Chicane::Vec3                        m_dragStartScale;
        Chicane::Rotator                     m_dragStartRotation;

        std::shared_ptr<bool>                m_bIsListening;
        Chicane::WindowEventSubscription     m_windowSubscription;
    };
}
