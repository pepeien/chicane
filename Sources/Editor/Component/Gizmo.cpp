#include "Editor/Component/Gizmo.reflected.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include <Chicane/Core/Input/Mouse/Button.hpp>
#include <Chicane/Core/Input/Mouse/Motion/Event.hpp>
#include <Chicane/Core/Input/Status.hpp>
#include <Chicane/Core/Math/Quat/QuatFloat.hpp>
#include <Chicane/Core/Math/Rotator.hpp>
#include <Chicane/Core/Math/Transform.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Grid/Component/Viewport.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>
#include <Chicane/Runtime/Scene/Trace/Request.hpp>

#include "Editor/UI/View/Home.hpp"

#include <SDL3/SDL.h>

namespace Editor
{
    static Chicane::FileSystem::Path axisMesh(GizmoType inType, const char* inAxis)
    {
        const char* folder = "Translation";
        switch (inType)
        {
        case GizmoType::Rotation:
            folder = "Rotation";

            break;

        case GizmoType::Scale:
            folder = "Scale";

            break;

        default:
            break;
        }

        return Chicane::String("Assets/Editor/Meshes/Gizmo/") + folder + "/" + inAxis + ".bmsh";
    }

    static bool closestOnAxis(
        const Chicane::Vec3& inOrigin,
        const Chicane::Vec3& inDirection,
        const Chicane::Vec3& inAxisOrigin,
        const Chicane::Vec3& inAxis,
        float                inMin,
        float                inMax,
        float&               outRay,
        float&               outAxis,
        float&               outDistance
    )
    {
        const Chicane::Vec3 offset = inOrigin - inAxisOrigin;
        const float         a      = inDirection.dot(inDirection);
        const float         b      = inDirection.dot(inAxis);
        const float         c      = inAxis.dot(inAxis);
        const float         d      = inDirection.dot(offset);
        const float         e      = inAxis.dot(offset);
        const float         denom  = a * c - b * b;

        float ray  = 0.0f;
        float axis = 0.0f;

        const bool bDenomAbove0p0001 = static_cast<bool>(std::fabs(denom) > 0.0001f);

        if (bDenomAbove0p0001)
        {
            ray  = (b * e - c * d) / denom;
            axis = (a * e - b * d) / denom;
        }

        if (!bDenomAbove0p0001)
        {
            axis = e;
            ray  = 0.0f;
        }

        ray  = std::max(ray, 0.0f);
        axis = std::clamp(axis, inMin, inMax);

        const Chicane::Vec3 delta = (inOrigin + inDirection * ray) - (inAxisOrigin + inAxis * axis);
        outRay                    = ray;
        outAxis                   = axis;
        outDistance               = delta.length();

        return true;
    }

    static bool intersectPlane(
        const Chicane::Vec3& inOrigin,
        const Chicane::Vec3& inDirection,
        const Chicane::Vec3& inPoint,
        const Chicane::Vec3& inNormal,
        Chicane::Vec3&       outHit
    )
    {
        const float denom = inDirection.dot(inNormal);
        if (std::fabs(denom) < 0.0001f)
        {
            return false;
        }

        const float ray = (inPoint - inOrigin).dot(inNormal) / denom;
        if (ray < 0.0f)
        {
            return false;
        }

        outHit = inOrigin + inDirection * ray;

        return true;
    }

    static bool closestOnRing(
        const Chicane::Vec3& inOrigin,
        const Chicane::Vec3& inDirection,
        const Chicane::Vec3& inCenter,
        const Chicane::Vec3& inAxis,
        float                inRadius,
        float                inTube,
        float&               outRay,
        float&               outDistance
    )
    {
        const Chicane::Vec3 axis = inAxis.normalize();
        Chicane::Vec3       hit  = Chicane::Vec3::sZero();
        if (intersectPlane(inOrigin, inDirection, inCenter, axis, hit))
        {
            const float distance = std::fabs((hit - inCenter).length() - inRadius);
            if (distance <= inTube)
            {
                outRay      = (hit - inOrigin).length();
                outDistance = distance;

                return true;
            }
        }

        const float         along  = std::max(0.0f, (inCenter - inOrigin).dot(inDirection));
        const Chicane::Vec3 query  = inOrigin + inDirection * along;
        Chicane::Vec3       planar = query - inCenter;
        planar                     = planar - axis * planar.dot(axis);
        const float   planarLength = planar.length();
        Chicane::Vec3 tangent      = axis.cross(Chicane::Vec3::sUp());
        if (tangent.dot(tangent) < 0.0001f)
        {
            tangent = axis.cross(Chicane::Vec3::sRight());
        }

        const Chicane::Vec3 onRing   = planarLength > 0.0001f ? inCenter + planar * (inRadius / planarLength)
                                                              : inCenter + tangent.normalize() * inRadius;
        const float         ray      = std::max(0.0f, (onRing - inOrigin).dot(inDirection));
        const float         distance = (inOrigin + inDirection * ray - onRing).length();
        if (distance > inTube)
        {
            return false;
        }

        outRay      = ray;
        outDistance = distance;

        return true;
    }

    static float angleOnPlane(const Chicane::Vec3& inPoint, const Chicane::Vec3& inOrigin, const Chicane::Vec3& inAxis)
    {
        Chicane::Vec3 tangent = inAxis.cross(Chicane::Vec3::sUp());
        if (tangent.dot(tangent) < 0.0001f)
        {
            tangent = inAxis.cross(Chicane::Vec3::sRight());
        }

        tangent                       = tangent.normalize();
        const Chicane::Vec3 bitangent = inAxis.cross(tangent).normalize();
        const Chicane::Vec3 offset    = inPoint - inOrigin;

        return std::atan2(offset.dot(bitangent), offset.dot(tangent));
    }

    static float safeDivide(float inValue, float inScale)
    {
        return std::fabs(inScale) > 0.0001f ? inValue / inScale : inValue;
    }

    static Chicane::Vec3 toRelativeDelta(const Chicane::Object& inItem, const Chicane::Vec3& inWorldDelta)
    {
        const Chicane::Vec3 local = inItem.getAbsoluteRotation().get().inverse() * inWorldDelta;
        const Chicane::Vec3 scale = inItem.getAbsoluteScale();

        return Chicane::Vec3(safeDivide(local.x, scale.x), safeDivide(local.y, scale.y), safeDivide(local.z, scale.z));
    }

    Gizmo::Gizmo()
        : Gizmo(GizmoType::Translation)
    {}

    Gizmo::Gizmo(GizmoType inType)
        : Chicane::Component(),
          m_bIsDragging(false),
          m_bIsListening(nullptr),
          m_type(inType),
          m_target(nullptr),
          m_origin(nullptr),
          m_x(nullptr),
          m_y(nullptr),
          m_z(nullptr),
          m_xy(nullptr),
          m_xz(nullptr),
          m_yz(nullptr),
          m_center(nullptr),
          m_hovered(nullptr),
          m_dragAxis(GizmoAxis::None),
          m_dragStartT(0.0f),
          m_dragStartAngle(0.0f),
          m_dragOrigin(Chicane::Vec3::sZero()),
          m_dragAxisDir(Chicane::Vec3::sRight()),
          m_dragStartHit(Chicane::Vec3::sZero()),
          m_dragStartTranslation(Chicane::Vec3::sZero()),
          m_dragStartScale(Chicane::Vec3::sOne()),
          m_dragStartRotation({}),
          m_cursor(Chicane::Vec2::sZero()),
          m_pawnSubscription()
    {
        setCanTick(true);
    }

    Gizmo::~Gizmo()
    {
        setHoveredHandle(nullptr);
        unbindController();
    }

    void Gizmo::onLoad()
    {
        m_origin = createHandle("Assets/Editor/Meshes/Gizmo/Origin.bmsh");

        m_x      = createHandle(axisMesh(m_type, "X"));
        m_y      = createHandle(axisMesh(m_type, "Y"));
        m_z      = createHandle(axisMesh(m_type, "Z"));
        m_xy     = createHandle("Assets/Editor/Meshes/Gizmo/Handle/XY.bmsh");
        m_xz     = createHandle("Assets/Editor/Meshes/Gizmo/Handle/XZ.bmsh");
        m_yz     = createHandle("Assets/Editor/Meshes/Gizmo/Handle/YZ.bmsh");
        m_center = createHandle("Assets/Editor/Meshes/Gizmo/Scale/Center.bmsh");

        applyMeshes();
        bindController();
        syncTransform();
    }

    void Gizmo::onUnload()
    {
        endDrag();
        setHoveredHandle(nullptr);
        unbindController();
    }

    void Gizmo::onTick(float inDeltaTime)
    {
        syncTransform();
    }

    GizmoType Gizmo::getType() const
    {
        return m_type;
    }

    void Gizmo::setType(GizmoType inType)
    {
        if (m_type == inType)
        {
            return;
        }

        endDrag();

        m_type = inType;
        applyMeshes();
    }

    Chicane::Object* Gizmo::getTarget() const
    {
        return m_target;
    }

    void Gizmo::setTarget(Chicane::Object* inTarget)
    {
        if (m_target == inTarget)
        {
            return;
        }

        endDrag();
        m_target = inTarget;
        syncTransform();
    }

    bool Gizmo::isDragging() const
    {
        return m_bIsDragging;
    }

    bool Gizmo::isHandleHovered() const
    {
        return m_hovered != nullptr;
    }

    bool Gizmo::isHandle(const Chicane::Object* inObject) const
    {
        return inObject && (inObject == m_origin || inObject == m_x || inObject == m_y || inObject == m_z ||
                            inObject == m_xy || inObject == m_xz || inObject == m_yz || inObject == m_center);
    }

    bool Gizmo::hitsHandle(const Chicane::SceneTraceRequest& inTrace) const
    {
        return hoveredHandle(inTrace) != nullptr;
    }

    bool Gizmo::isViewportHovered() const
    {
        std::shared_ptr<Chicane::Grid::View> view = Chicane::Instance::sInstance().getView();
        if (!view)
        {
            return true;
        }

        Chicane::Grid::Component* hovered = view->getHovered();

        if (!hovered)
        {
            return false;
        }

        for (Chicane::Grid::Component* node = hovered; node != nullptr; node = node->getParent())
        {
            if (node->getTag().equals(Chicane::Grid::Viewport::TAG_ID))
            {
                return !node->getAttribute(Chicane::Grid::Component::ON_HOVER_ATTRIBUTE_NAME).isEmpty();
            }

            if (node->isRoot())
            {
                break;
            }
        }

        return false;
    }

    bool Gizmo::isRelativeSpace() const
    {
        if (std::shared_ptr<HomeView> home = Chicane::Instance::sInstance().getView<HomeView>())
        {
            return home->getCoordinateSpace() == CoordinateSpace::Relative;
        }

        return false;
    }

    void Gizmo::onCursor(const Chicane::Vec2& inLocation)
    {
        if (getScene() != Chicane::Instance::sInstance().getScene().get())
        {
            if (!m_bIsDragging)
            {
                setHoveredHandle(nullptr);
            }

            return;
        }

        Chicane::Window* window = Chicane::Instance::sInstance().getWindow();
        if (!window)
        {
            if (!m_bIsDragging)
            {
                setHoveredHandle(nullptr);
            }

            return;
        }

        if (m_bIsDragging)
        {
            Chicane::SceneTraceRequest trace;
            if (cursorTrace(inLocation, trace))
            {
                drag(trace);
            }

            window->setCursor(Chicane::WindowCursor::Pointer);

            return;
        }

        bool overHandle = false;

        const bool bNotFocused =
            static_cast<bool>(!window->isFocused() && !window->isTextInputActive() && isViewportHovered());

        if (bNotFocused)
        {
            Chicane::SceneTraceRequest trace;
            const bool                 bCursorTrace = static_cast<bool>(cursorTrace(inLocation, trace));

            if (bCursorTrace)
            {
                setHoveredHandle(hoveredHandle(trace));
                overHandle = m_hovered != nullptr;
            }

            if (!bCursorTrace)
            {
                setHoveredHandle(nullptr);
            }
        }

        if (!bNotFocused)
        {
            setHoveredHandle(nullptr);
        }

        if (overHandle)
        {
            window->setCursor(Chicane::WindowCursor::Pointer);

            return;
        }

        if (std::shared_ptr<Chicane::Grid::View> view = Chicane::Instance::sInstance().getView())
        {
            window->setCursor(view->getPointer());

            return;
        }

        window->setCursor(Chicane::WindowCursor::Default);
    }

    void Gizmo::onPress()
    {
        if (m_bIsDragging || !m_target)
        {
            return;
        }

        Chicane::Window* window = Chicane::Instance::sInstance().getWindow();
        if (!window || window->isFocused() || window->isTextInputActive() || !isViewportHovered())
        {
            return;
        }

        Chicane::SceneTraceRequest trace;
        if (!cursorTrace(m_cursor, trace))
        {
            return;
        }

        beginDrag(trace);
    }

    void Gizmo::onRelease()
    {
        const bool bWasDragging = m_bIsDragging;
        endDrag();

        if (bWasDragging)
        {
            onCursor(m_cursor);
        }
    }

    void Gizmo::syncTransform()
    {
        if (!m_target)
        {
            endDrag();
            setHoveredHandle(nullptr);
            refreshHandleVisibility();

            return;
        }

        if (m_bIsDragging)
        {
            setAbsoluteScale(Chicane::Vec3(handleScale()));
            if (m_type == GizmoType::Translation)
            {
                setAbsoluteTranslation(m_target->getTranslation());
            }

            syncOrigin();

            return;
        }

        Chicane::Transform pose;
        pose.setTranslation(m_target->getTranslation());
        pose.setRotation(isRelativeSpace() ? m_target->getRotation() : Chicane::Rotator());
        pose.setScale(Chicane::Vec3(handleScale()));
        setAbsolute(pose);

        syncOrigin();
        refreshHandleVisibility();
    }

    void Gizmo::syncOrigin()
    {
        if (!m_origin)
        {
            return;
        }

        Chicane::CCamera* camera = activeCamera();
        if (!camera)
        {
            return;
        }

        m_origin->lookAt(camera->getTranslation());
    }

    void Gizmo::applyMeshes()
    {
        if (m_x)
        {
            m_x->setMesh(axisMesh(m_type, "X"));
        }

        if (m_y)
        {
            m_y->setMesh(axisMesh(m_type, "Y"));
        }

        if (m_z)
        {
            m_z->setMesh(axisMesh(m_type, "Z"));
        }

        const float gap = m_type == GizmoType::Translation ? AXIS_GAP : 0.0f;
        if (m_x)
        {
            m_x->setRelativeTranslation(Chicane::Vec3(gap, 0.0f, 0.0f));
        }

        if (m_y)
        {
            m_y->setRelativeTranslation(Chicane::Vec3(0.0f, gap, 0.0f));
        }

        if (m_z)
        {
            m_z->setRelativeTranslation(Chicane::Vec3(0.0f, 0.0f, gap));
        }

        refreshHandleVisibility();
    }

    Chicane::CCamera* Gizmo::activeCamera() const
    {
        Chicane::Scene* scene = getScene();
        if (!scene)
        {
            return nullptr;
        }

        std::vector<Chicane::CCamera*> cameras = scene->getActiveComponents<Chicane::CCamera>();
        if (cameras.empty())
        {
            return nullptr;
        }

        return cameras.back();
    }

    void Gizmo::bindController()
    {
        unbindController();

        Chicane::Controller* controller = Chicane::Instance::sInstance().getController();
        if (!controller)
        {
            return;
        }

        m_bIsListening                  = std::make_shared<bool>(true);
        std::shared_ptr<bool> listening = m_bIsListening;

        m_pawnSubscription = controller->watchAttachment(
            [this, listening](Chicane::APawn* inPawn)
            {
                if (!listening || !*listening || !inPawn)
                {
                    return;
                }

                bindInput(Chicane::Instance::sInstance().getController());
            }
        );
    }

    void Gizmo::unbindController()
    {
        if (m_bIsListening)
        {
            *m_bIsListening = false;
        }

        m_pawnSubscription.complete();
    }

    void Gizmo::bindInput(Chicane::Controller* inController)
    {
        if (!inController)
        {
            return;
        }

        std::shared_ptr<bool> listening = m_bIsListening;

        inController->bindEvent(
            [this, listening](const Chicane::Input::MouseMotionEvent& inEvent)
            {
                if (!listening || !*listening)
                {
                    return;
                }

                m_cursor = inEvent.location;
                onCursor(m_cursor);
            }
        );

        inController->bindEvent(
            Chicane::Input::MouseButton::Left,
            Chicane::Input::Status::Pressed,
            [this, listening]()
            {
                if (!listening || !*listening)
                {
                    return;
                }

                onPress();
            }
        );
        inController->bindEvent(
            Chicane::Input::MouseButton::Left,
            Chicane::Input::Status::Released,
            [this, listening]()
            {
                if (!listening || !*listening)
                {
                    return;
                }

                onRelease();
            }
        );
    }

    void Gizmo::setHandleActive(Chicane::CMesh* inMesh, bool inValue)
    {
        if (!inMesh)
        {
            return;
        }

        if (!inValue && m_hovered == inMesh)
        {
            setHoveredHandle(nullptr);
        }

        if (inValue)
        {
            inMesh->activate();

            return;
        }

        inMesh->deactivate();
    }

    void Gizmo::refreshHandleVisibility()
    {
        const bool bShow       = m_target != nullptr;
        const bool bShowPlanes = bShow && m_type != GizmoType::Rotation;

        setHandleActive(m_xy, bShowPlanes);
        setHandleActive(m_xz, bShowPlanes);
        setHandleActive(m_yz, bShowPlanes);
        setHandleActive(m_center, bShow && m_type == GizmoType::Scale);
        setHandleActive(m_origin, bShow);
        setHandleActive(m_x, bShow);
        setHandleActive(m_y, bShow);
        setHandleActive(m_z, bShow);
    }

    GizmoAxis Gizmo::axisFromHandle(const Chicane::CMesh* inHandle) const
    {
        if (!inHandle)
        {
            return GizmoAxis::None;
        }

        if (inHandle == m_x)
        {
            return GizmoAxis::X;
        }

        if (inHandle == m_y)
        {
            return GizmoAxis::Y;
        }

        if (inHandle == m_z)
        {
            return GizmoAxis::Z;
        }

        if (inHandle == m_xy)
        {
            return GizmoAxis::XY;
        }

        if (inHandle == m_xz)
        {
            return GizmoAxis::XZ;
        }

        if (inHandle == m_yz)
        {
            return GizmoAxis::YZ;
        }

        if (inHandle == m_center)
        {
            return GizmoAxis::Center;
        }

        if (inHandle == m_origin && m_type != GizmoType::Rotation)
        {
            return GizmoAxis::Center;
        }

        return GizmoAxis::None;
    }

    Chicane::CMesh* Gizmo::createHandle(const Chicane::FileSystem::Path& inMesh)
    {
        Chicane::CMesh* mesh = getScene()->createComponent<Chicane::CMesh>();
        mesh->setCanCastShadows(false);
        mesh->setIsLit(false);
        mesh->setIsForeground(true);
        mesh->setIsTransient(true);
        mesh->setMesh(inMesh);
        mesh->attachTo(this);
        mesh->activate();

        return mesh;
    }

    Chicane::CMesh* Gizmo::hoveredHandle(const Chicane::SceneTraceRequest& inTrace) const
    {
        if (!m_target || !inTrace.isValid())
        {
            return nullptr;
        }

        const Chicane::Vec3 origin = getTranslation();
        const Chicane::Vec3 rayO   = inTrace.getOrigin();
        const Chicane::Vec3 rayD   = inTrace.getDirection();
        const float         scale  = handleScale();
        if (scale <= 0.0f)
        {
            return nullptr;
        }

        Chicane::CMesh* bestHandle   = nullptr;
        float           bestRay      = 1.0e9f;
        float           bestDistance = 1.0e9f;

        const auto consider = [&](Chicane::CMesh* inMesh, float inDistance, float inRay)
        {
            if (!inMesh || !inMesh->isActive() || inRay < 0.0f)
            {
                return;
            }

            if (inRay > bestRay + 0.0001f)
            {
                return;
            }

            if (std::fabs(inRay - bestRay) <= 0.0001f && inDistance >= bestDistance)
            {
                return;
            }

            bestHandle   = inMesh;
            bestRay      = inRay;
            bestDistance = inDistance;
        };

        const auto pickAxis = [&](Chicane::CMesh* inMesh, GizmoAxis inAxis)
        {
            if (m_type == GizmoType::Rotation)
            {
                float ray      = 0.0f;
                float distance = 0.0f;
                if (closestOnRing(
                        rayO,
                        rayD,
                        origin,
                        axisDirection(inAxis),
                        RING_RADIUS * scale,
                        RING_TUBE * scale,
                        ray,
                        distance
                    ))
                {
                    consider(inMesh, distance, ray);
                }

                return;
            }

            float ray      = 0.0f;
            float along    = 0.0f;
            float distance = 0.0f;
            closestOnAxis(
                rayO,
                rayD,
                origin,
                axisDirection(inAxis),
                AXIS_START * scale,
                AXIS_END * scale,
                ray,
                along,
                distance
            );
            if (distance <= AXIS_PICK * scale)
            {
                consider(inMesh, distance, ray);
            }
        };

        const auto pickPlane = [&](Chicane::CMesh* inMesh, const Chicane::Vec3& inU, const Chicane::Vec3& inV)
        {
            const Chicane::Vec3 normal = inU.cross(inV).normalize();
            Chicane::Vec3       point  = Chicane::Vec3::sZero();
            if (!intersectPlane(rayO, rayD, origin, normal, point))
            {
                return;
            }

            const Chicane::Vec3 offset = point - origin;
            const float         u      = offset.dot(inU.normalize()) / scale;
            const float         v      = offset.dot(inV.normalize()) / scale;
            if (u < PLANE_INNER || u > PLANE_OUTER || v < PLANE_INNER || v > PLANE_OUTER)
            {
                return;
            }

            consider(inMesh, 0.0f, (point - rayO).length());
        };

        pickAxis(m_x, GizmoAxis::X);
        pickAxis(m_y, GizmoAxis::Y);
        pickAxis(m_z, GizmoAxis::Z);

        if (m_type != GizmoType::Rotation)
        {
            pickPlane(m_xy, getRight(), getForward());
            pickPlane(m_xz, getRight(), getUp());
            pickPlane(m_yz, getForward(), getUp());
        }

        if (m_origin && m_type != GizmoType::Rotation)
        {
            Chicane::CCamera* camera = activeCamera();
            if (camera)
            {
                Chicane::Vec3 point = Chicane::Vec3::sZero();
                if (intersectPlane(rayO, rayD, origin, camera->getForward().normalize(), point))
                {
                    const float distance = std::fabs((point - origin).length() - ORIGIN_RADIUS * scale);
                    if (distance <= ORIGIN_PICK * scale)
                    {
                        consider(m_origin, distance, (point - rayO).length());
                    }
                }
            }
        }

        if (m_center && m_type == GizmoType::Scale)
        {
            const float ray      = std::max(0.0f, (origin - rayO).dot(rayD));
            const float distance = (rayO + rayD * ray - origin).length();
            if (distance <= CENTER_PICK * scale)
            {
                consider(m_center, distance, ray);
            }
        }

        return bestHandle;
    }

    void Gizmo::setHoveredHandle(Chicane::CMesh* inHandle)
    {
        if (m_hovered == inHandle)
        {
            return;
        }

        if (m_hovered)
        {
            m_hovered->setEmissiveStrength(1.0f);
        }

        m_hovered = inHandle;

        if (m_hovered)
        {
            m_hovered->setEmissiveStrength(HANDLE_GLOW);
        }
    }

    Chicane::Vec3 Gizmo::axisDirection(GizmoAxis inAxis) const
    {
        switch (inAxis)
        {
        case GizmoAxis::Y:
            return getForward().normalize();

        case GizmoAxis::Z:
            return getUp().normalize();

        default:
            return getRight().normalize();
        }
    }

    Chicane::Vec3 Gizmo::planeNormal(GizmoAxis inAxis) const
    {
        switch (inAxis)
        {
        case GizmoAxis::XY:
            return getUp().normalize();

        case GizmoAxis::XZ:
            return getForward().normalize();

        case GizmoAxis::YZ:
            return getRight().normalize();

        default:
            return getUp().normalize();
        }
    }

    bool Gizmo::cursorTrace(const Chicane::Vec2& inLocation, Chicane::SceneTraceRequest& outTrace) const
    {
        Chicane::Scene* scene = getScene();
        if (!scene)
        {
            return false;
        }

        return scene
            ->trace(outTrace, inLocation, Chicane::Instance::sInstance().getScreenViewportRect(), activeCamera());
    }

    void Gizmo::beginDrag(const Chicane::SceneTraceRequest& inTrace)
    {
        if (!m_target)
        {
            return;
        }

        Chicane::CMesh* handle = hoveredHandle(inTrace);
        const GizmoAxis axis   = axisFromHandle(handle);
        if (axis == GizmoAxis::None)
        {
            return;
        }

        setHoveredHandle(handle);

        const Chicane::Vec3 origin    = getTranslation();
        const Chicane::Vec3 rayOrigin = inTrace.getOrigin();
        const Chicane::Vec3 rayDir    = inTrace.getDirection();

        m_dragAxis       = axis;
        m_dragOrigin     = origin;
        m_dragStartHit   = origin;
        m_dragStartT     = MIN_SCALE;
        m_dragStartAngle = 0.0f;

        const bool bAxisCenter = static_cast<bool>(axis == GizmoAxis::Center);

        if (bAxisCenter)
        {
            const bool bTypeScale = static_cast<bool>(m_type == GizmoType::Scale);

            if (bTypeScale)
            {
                const float ray      = std::max(0.0f, (origin - rayOrigin).dot(rayDir));
                const float distance = (rayOrigin + rayDir * ray - origin).length();
                m_dragStartT         = std::max(distance, MIN_SCALE);
                m_dragAxisDir        = Chicane::Vec3::sOne();
            }

            if (!bTypeScale)
            {
                Chicane::CCamera* camera = activeCamera();
                if (!camera)
                {
                    return;
                }

                m_dragAxisDir = camera->getForward().normalize();
                if (!intersectPlane(rayOrigin, rayDir, origin, m_dragAxisDir, m_dragStartHit))
                {
                    return;
                }
            }
        }

        const bool bPlaneAxis = !bAxisCenter && (isPlaneAxis(axis));

        if (bPlaneAxis)
        {
            m_dragAxisDir = planeNormal(axis);
            if (!intersectPlane(rayOrigin, rayDir, origin, m_dragAxisDir, m_dragStartHit))
            {
                return;
            }

            m_dragStartT = std::max((m_dragStartHit - origin).length(), MIN_SCALE);
        }

        const bool bTypeRotation = !bAxisCenter && !bPlaneAxis && (m_type == GizmoType::Rotation);

        if (bTypeRotation)
        {
            m_dragAxisDir = axisDirection(axis);
            Chicane::Vec3 point;
            if (!intersectPlane(rayOrigin, rayDir, origin, m_dragAxisDir, point))
            {
                return;
            }

            m_dragStartAngle = angleOnPlane(point, origin, m_dragAxisDir);
            m_dragStartHit   = point;
        }

        if (!bAxisCenter && !bPlaneAxis && !bTypeRotation)
        {
            m_dragAxisDir  = axisDirection(axis);
            float ray      = 0.0f;
            float along    = 0.0f;
            float distance = 0.0f;
            closestOnAxis(rayOrigin, rayDir, origin, m_dragAxisDir, -1000.0f, 1000.0f, ray, along, distance);
            m_dragStartT = along;
        }

        m_bIsDragging = true;
        captureDragStart();
    }

    void Gizmo::drag(const Chicane::SceneTraceRequest& inTrace)
    {
        if (!m_bIsDragging || !m_target)
        {
            return;
        }

        const Chicane::Vec3 rayOrigin = inTrace.getOrigin();
        const Chicane::Vec3 rayDir    = inTrace.getDirection();

        if (m_type == GizmoType::Rotation)
        {
            Chicane::Vec3 point = Chicane::Vec3::sZero();
            if (!intersectPlane(rayOrigin, rayDir, m_dragOrigin, m_dragAxisDir, point))
            {
                return;
            }

            applyRotationDelta(angleOnPlane(point, m_dragOrigin, m_dragAxisDir) - m_dragStartAngle);

            return;
        }

        if (m_dragAxis == GizmoAxis::Center)
        {
            if (m_type == GizmoType::Scale)
            {
                const float ray      = std::max(0.0f, (m_dragOrigin - rayOrigin).dot(rayDir));
                const float distance = std::max((rayOrigin + rayDir * ray - m_dragOrigin).length(), MIN_SCALE);
                applyScaleValue(m_dragStartScale * (distance / std::max(m_dragStartT, MIN_SCALE)));
                poseHandles();

                return;
            }

            Chicane::Vec3 point = Chicane::Vec3::sZero();
            if (!intersectPlane(rayOrigin, rayDir, m_dragOrigin, m_dragAxisDir, point))
            {
                return;
            }

            applyTranslationDelta(point - m_dragStartHit);
            poseHandles();

            return;
        }

        if (isPlaneAxis(m_dragAxis))
        {
            Chicane::Vec3 point = Chicane::Vec3::sZero();
            if (!intersectPlane(rayOrigin, rayDir, m_dragOrigin, m_dragAxisDir, point))
            {
                return;
            }

            if (m_type == GizmoType::Scale)
            {
                const float   ratio = (point - m_dragOrigin).length() / std::max(m_dragStartT, MIN_SCALE);
                Chicane::Vec3 scale = m_dragStartScale;

                if (m_dragAxis == GizmoAxis::XY || m_dragAxis == GizmoAxis::XZ)
                {
                    scale.x = std::max(MIN_SCALE, m_dragStartScale.x * ratio);
                }

                if (m_dragAxis == GizmoAxis::XY || m_dragAxis == GizmoAxis::YZ)
                {
                    scale.y = std::max(MIN_SCALE, m_dragStartScale.y * ratio);
                }

                if (m_dragAxis == GizmoAxis::XZ || m_dragAxis == GizmoAxis::YZ)
                {
                    scale.z = std::max(MIN_SCALE, m_dragStartScale.z * ratio);
                }

                applyScaleValue(scale);

                return;
            }

            applyTranslationDelta(point - m_dragStartHit);
            poseHandles();

            return;
        }

        float ray      = 0.0f;
        float along    = 0.0f;
        float distance = 0.0f;
        closestOnAxis(rayOrigin, rayDir, m_dragOrigin, m_dragAxisDir, -1000.0f, 1000.0f, ray, along, distance);

        if (m_type == GizmoType::Scale)
        {
            const float   ratio = along / std::max(m_dragStartT, MIN_SCALE);
            Chicane::Vec3 scale = m_dragStartScale;

            switch (m_dragAxis)
            {
            case GizmoAxis::X:
                scale.x = std::max(MIN_SCALE, m_dragStartScale.x * ratio);

                break;

            case GizmoAxis::Y:
                scale.y = std::max(MIN_SCALE, m_dragStartScale.y * ratio);

                break;

            default:
                scale.z = std::max(MIN_SCALE, m_dragStartScale.z * ratio);

                break;
            }

            applyScaleValue(scale);

            return;
        }

        applyTranslationDelta(m_dragAxisDir * (along - m_dragStartT));
        poseHandles();
    }

    void Gizmo::endDrag()
    {
        if (!m_bIsDragging)
        {
            return;
        }

        m_bIsDragging = false;
        m_dragAxis    = GizmoAxis::None;
        syncTransform();
    }

    void Gizmo::captureDragStart()
    {
        if (!m_target)
        {
            return;
        }

        if (isRelativeSpace())
        {
            m_dragStartTranslation = m_target->getRelativeTranslation();
            m_dragStartScale       = m_target->getRelativeScale();
            m_dragStartRotation    = m_target->getRelativeRotation();

            return;
        }

        m_dragStartTranslation = m_target->getTranslation();
        m_dragStartScale       = m_target->getAbsoluteScale();
        m_dragStartRotation    = m_target->getAbsoluteRotation();
    }

    void Gizmo::poseHandles()
    {
        if (!m_target)
        {
            return;
        }

        setAbsoluteTranslation(m_target->getTranslation());
        setAbsoluteScale(Chicane::Vec3(handleScale()));
        syncOrigin();
    }

    void Gizmo::applyTranslationDelta(const Chicane::Vec3& inWorldDelta)
    {
        if (!m_target)
        {
            return;
        }

        if (isRelativeSpace())
        {
            m_target->setRelativeTranslation(m_dragStartTranslation + toRelativeDelta(*m_target, inWorldDelta));

            return;
        }

        m_target->setTranslation(m_dragStartTranslation + inWorldDelta);
    }

    void Gizmo::applyRotationDelta(float inDelta)
    {
        if (!m_target)
        {
            return;
        }

        const Chicane::QuatFloat delta = Chicane::QuatFloat::sFromAxis(m_dragAxisDir, inDelta);
        if (isRelativeSpace())
        {
            m_target->setRelativeRotation(m_dragStartRotation);
            const Chicane::Vec3 localAxis = m_target->getRotation().get().inverse() * m_dragAxisDir;
            m_target->addRelativeRotation(Chicane::QuatFloat::sFromAxis(localAxis, inDelta));

            return;
        }

        m_target->setAbsoluteRotation(m_dragStartRotation);
        m_target->addAbsoluteRotation(delta);
    }

    void Gizmo::applyScaleValue(const Chicane::Vec3& inScale)
    {
        if (!m_target)
        {
            return;
        }

        if (isRelativeSpace())
        {
            m_target->setRelativeScale(inScale);

            return;
        }

        m_target->setAbsoluteScale(inScale);
    }

    float Gizmo::handleScale() const
    {
        if (!m_target)
        {
            return 1.0f;
        }

        Chicane::CCamera* camera = activeCamera();
        if (!camera)
        {
            return 1.0f;
        }

        const Chicane::Vec3 offset = m_target->getTranslation() - camera->getTranslation();

        return std::clamp(offset.length() * HANDLE_SCALE, MIN_HANDLE, MAX_HANDLE);
    }
}
