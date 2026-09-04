#include "Editor/Component/Gizmo.reflected.hpp"

#include <algorithm>
#include <cmath>

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Input/Keyboard/Event.hpp>
#include <Chicane/Core/Input/Mouse/Button/Event.hpp>
#include <Chicane/Core/Input/Mouse/Motion/Event.hpp>
#include <Chicane/Core/Math/Quat/QuatFloat.hpp>
#include <Chicane/Core/Window.hpp>
#include <Chicane/Grid/Component/Viewport.hpp>
#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>
#include <Chicane/Runtime/Scene/Component/Camera.hpp>

#include "Editor/Actor/Item.hpp"
#include "Editor/UI/View/Home.hpp"

namespace Editor
{
    namespace
    {
        constexpr float AXIS_LENGTH    = 1.5f;
        constexpr float AXIS_RADIUS    = 0.12f;
        constexpr float RING_RADIUS    = 1.0f;
        constexpr float RING_THICKNESS = 0.12f;
        constexpr float CENTER_RADIUS  = 0.16f;
        constexpr float MIN_SCALE      = 0.01f;

        Chicane::FileSystem::Path meshPath(GizmoType inType)
        {
            switch (inType)
            {
            case GizmoType::Rotation:
                return "Assets/Editor/Meshes/Gizmo/Rotation.bmsh";

            case GizmoType::Scale:
                return "Assets/Editor/Meshes/Gizmo/Scale.bmsh";

            default:
                return "Assets/Editor/Meshes/Gizmo/Translation.bmsh";
            }
        }

        float length(const Chicane::Vec3& inValue)
        {
            return std::sqrt(inValue.dot(inValue));
        }

        bool closestOnAxis(
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

            if (std::fabs(denom) > 0.0001f)
            {
                ray  = (b * e - c * d) / denom;
                axis = (a * e - b * d) / denom;
            }
            else
            {
                axis = e;
                ray  = 0.0f;
            }

            ray  = std::max(ray, 0.0f);
            axis = std::clamp(axis, inMin, inMax);

            const Chicane::Vec3 delta = (inOrigin + inDirection * ray) - (inAxisOrigin + inAxis * axis);
            outRay                    = ray;
            outAxis                   = axis;
            outDistance               = length(delta);

            return true;
        }

        bool intersectPlane(
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

        float angleOnPlane(const Chicane::Vec3& inPoint, const Chicane::Vec3& inOrigin, const Chicane::Vec3& inAxis)
        {
            Chicane::Vec3 tangent = inAxis.cross(Chicane::Vec3::Up());
            if (tangent.dot(tangent) < 0.0001f)
            {
                tangent = inAxis.cross(Chicane::Vec3::Right());
            }

            tangent               = tangent.normalize();
            const Chicane::Vec3 bitangent = inAxis.cross(tangent).normalize();
            const Chicane::Vec3 offset    = inPoint - inOrigin;

            return std::atan2(offset.dot(bitangent), offset.dot(tangent));
        }
    }

    Gizmo::Gizmo()
        : Gizmo(GizmoType::Translation)
    {}

    Gizmo::Gizmo(GizmoType inType)
        : Chicane::Component(),
          m_type(inType),
          m_mesh(nullptr),
          m_target(nullptr),
          m_targetSubscription({}),
          m_bIsDragging(false),
          m_dragAxis(GizmoAxis::None),
          m_dragStartT(0.0f),
          m_dragStartAngle(0.0f),
          m_dragOrigin(Chicane::Vec3::Zero()),
          m_dragAxisDir(Chicane::Vec3::Right()),
          m_dragStartTranslation(Chicane::Vec3::Zero()),
          m_dragStartScale(Chicane::Vec3::One()),
          m_dragStartRotation({}),
          m_bIsListening(nullptr),
          m_windowSubscription({})
    {}

    Gizmo::~Gizmo()
    {
        unbindWindow();
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

        applyMesh();
    }

    Chicane::Object* Gizmo::getTarget() const
    {
        return m_target;
    }

    void Gizmo::setTarget(Chicane::Object* inTarget)
    {
        if (m_target == inTarget)
        {
            if (m_target)
            {
                activate();
            }

            return;
        }

        endDrag();
        m_targetSubscription.complete();
        m_target = inTarget;

        if (!m_target)
        {
            deactivate();

            return;
        }

        m_targetSubscription = m_target->watchChanges([this]() { syncTransform(); });
        syncTransform();
        activate();
    }

    bool Gizmo::isDragging() const
    {
        return m_bIsDragging;
    }

    bool Gizmo::pick(const Chicane::Vec3& inOrigin, const Chicane::Vec3& inDirection)
    {
        if (!isActive() || !m_target)
        {
            return false;
        }

        const Chicane::Vec3 origin = getTranslation();
        const GizmoAxis     axes[] = {GizmoAxis::X, GizmoAxis::Y, GizmoAxis::Z};

        GizmoAxis hit      = GizmoAxis::None;
        float     best     = m_type == GizmoType::Rotation ? RING_THICKNESS : AXIS_RADIUS;
        float     startT   = 0.0f;
        float     startAng = 0.0f;
        Chicane::Vec3 hitAxis = Chicane::Vec3::Right();

        if (m_type == GizmoType::Scale)
        {
            const float ray = std::max(0.0f, (origin - inOrigin).dot(inDirection));
            const float distance = length((inOrigin + inDirection * ray) - origin);
            if (distance < CENTER_RADIUS)
            {
                hit      = GizmoAxis::Center;
                best     = distance;
                startT   = std::max(distance, MIN_SCALE);
                hitAxis  = Chicane::Vec3::One();
            }
        }

        for (GizmoAxis axis : axes)
        {
            const Chicane::Vec3 direction = axisDirection(axis);

            if (m_type == GizmoType::Rotation)
            {
                Chicane::Vec3 point = Chicane::Vec3::Zero();
                if (!intersectPlane(inOrigin, inDirection, origin, direction, point))
                {
                    continue;
                }

                const float distance = std::fabs(length(point - origin) - RING_RADIUS);
                if (distance >= best)
                {
                    continue;
                }

                hit      = axis;
                best     = distance;
                startAng = angleOnPlane(point, origin, direction);
                hitAxis  = direction;

                continue;
            }

            float ray      = 0.0f;
            float along    = 0.0f;
            float distance = 0.0f;
            closestOnAxis(
                inOrigin,
                inDirection,
                origin,
                direction,
                0.0f,
                AXIS_LENGTH,
                ray,
                along,
                distance
            );

            if (distance >= best)
            {
                continue;
            }

            if (along < 0.2f)
            {
                continue;
            }

            hit     = axis;
            best    = distance;
            startT  = along;
            hitAxis = direction;
        }

        if (hit == GizmoAxis::None)
        {
            return false;
        }

        m_bIsDragging           = true;
        m_dragAxis              = hit;
        m_dragStartT            = startT;
        m_dragStartAngle        = startAng;
        m_dragOrigin            = origin;
        m_dragAxisDir           = hitAxis;
        m_dragStartTranslation  = m_target->getTranslation();
        m_dragStartScale        = m_target->getScale();
        m_dragStartRotation     = m_target->getRotation();

        return true;
    }

    void Gizmo::drag(const Chicane::Vec3& inOrigin, const Chicane::Vec3& inDirection)
    {
        if (!m_bIsDragging || !m_target)
        {
            return;
        }

        if (m_type == GizmoType::Rotation)
        {
            Chicane::Vec3 point = Chicane::Vec3::Zero();
            if (!intersectPlane(inOrigin, inDirection, m_dragOrigin, m_dragAxisDir, point))
            {
                return;
            }

            const float delta = angleOnPlane(point, m_dragOrigin, m_dragAxisDir) - m_dragStartAngle;

            m_target->setAbsoluteRotation(m_dragStartRotation);
            m_target->addAbsoluteRotation(Chicane::QuatFloat::fromAxis(m_dragAxisDir, delta));

            return;
        }

        if (m_dragAxis == GizmoAxis::Center)
        {
            const float ray      = std::max(0.0f, (m_dragOrigin - inOrigin).dot(inDirection));
            const float distance = std::max(length((inOrigin + inDirection * ray) - m_dragOrigin), MIN_SCALE);
            const float ratio    = distance / std::max(m_dragStartT, MIN_SCALE);

            m_target->setAbsoluteScale(m_dragStartScale * ratio);
            setAbsoluteTranslation(m_target->getTranslation());
            setAbsoluteRotation(m_target->getRotation());
            setAbsoluteScale(Chicane::Vec3::One());

            return;
        }

        float ray      = 0.0f;
        float along    = 0.0f;
        float distance = 0.0f;
        closestOnAxis(
            inOrigin,
            inDirection,
            m_dragOrigin,
            m_dragAxisDir,
            -1000.0f,
            1000.0f,
            ray,
            along,
            distance
        );

        if (m_type == GizmoType::Scale)
        {
            const float ratio = along / std::max(m_dragStartT, MIN_SCALE);
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

            m_target->setAbsoluteScale(scale);

            return;
        }

        m_target->setAbsoluteTranslation(
            m_dragStartTranslation + m_dragAxisDir * (along - m_dragStartT)
        );
        setAbsoluteTranslation(m_target->getTranslation());
        setAbsoluteRotation(m_target->getRotation());
        setAbsoluteScale(Chicane::Vec3::One());
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

    void Gizmo::onLoad()
    {
        m_mesh = getScene()->createComponent<Chicane::CMesh>();
        m_mesh->attachTo(this);
        applyMesh();
        bindWindow();
    }

    void Gizmo::onUnload()
    {
        unbindWindow();
    }

    void Gizmo::onActivation()
    {
        if (m_mesh)
        {
            m_mesh->activate();
        }
    }

    void Gizmo::onDeactivation()
    {
        endDrag();

        if (m_mesh)
        {
            m_mesh->deactivate();
        }
    }

    void Gizmo::applyMesh()
    {
        if (!m_mesh)
        {
            return;
        }

        m_mesh->setMesh(meshPath(m_type));
    }

    void Gizmo::syncTransform()
    {
        if (!m_target || m_bIsDragging)
        {
            return;
        }

        setAbsoluteTranslation(m_target->getTranslation());
        setAbsoluteRotation(m_target->getRotation());
        setAbsoluteScale(Chicane::Vec3::One());
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

    void Gizmo::bindWindow()
    {
        unbindWindow();

        Chicane::Window* window = Chicane::Application::getInstance().getWindow();
        if (!window)
        {
            return;
        }

        m_bIsListening = std::make_shared<bool>(true);
        std::shared_ptr<bool> listening = m_bIsListening;

        m_windowSubscription = window->watchEvent(
            [this, listening](const Chicane::WindowEvent& inEvent)
            {
                if (!listening || !*listening)
                {
                    return;
                }

                onWindowEvent(inEvent);
            }
        );
    }

    void Gizmo::unbindWindow()
    {
        if (m_bIsListening)
        {
            *m_bIsListening = false;
        }

        m_windowSubscription.complete();
    }

    void Gizmo::onWindowEvent(const Chicane::WindowEvent& inEvent)
    {
        Chicane::Window* window = Chicane::Application::getInstance().getWindow();
        if (!window || window->isFocused() || window->isTextInputActive())
        {
            if (isDragging())
            {
                endDrag();
            }

            return;
        }

        switch (inEvent.type)
        {
        case Chicane::WindowEventType::MouseButtonDown:
        {
            const Chicane::Input::MouseButtonEvent event =
                *static_cast<const Chicane::Input::MouseButtonEvent*>(inEvent.data);
            if (event.button != Chicane::Input::MouseButton::Left)
            {
                break;
            }

            pickAt(event.location);

            break;
        }

        case Chicane::WindowEventType::MouseButtonUp:
        {
            const Chicane::Input::MouseButtonEvent event =
                *static_cast<const Chicane::Input::MouseButtonEvent*>(inEvent.data);
            if (event.button != Chicane::Input::MouseButton::Left)
            {
                break;
            }

            endDrag();

            break;
        }

        case Chicane::WindowEventType::MouseMotion:
        {
            if (!isDragging())
            {
                break;
            }

            const Chicane::Input::MouseMotionEvent event =
                *static_cast<const Chicane::Input::MouseMotionEvent*>(inEvent.data);
            dragAt(event.location);

            break;
        }

        case Chicane::WindowEventType::KeyDown:
        {
            if (!getTarget())
            {
                break;
            }

            const Chicane::Input::KeyboardEvent event =
                *static_cast<const Chicane::Input::KeyboardEvent*>(inEvent.data);
            if (event.bIsRepeating)
            {
                break;
            }

            switch (event.button)
            {
            case Chicane::Input::KeyboardButton::Number1:
            case Chicane::Input::KeyboardButton::T:
                setType(GizmoType::Translation);
                break;

            case Chicane::Input::KeyboardButton::Number2:
            case Chicane::Input::KeyboardButton::R:
                setType(GizmoType::Rotation);
                break;

            case Chicane::Input::KeyboardButton::Number3:
            case Chicane::Input::KeyboardButton::E:
                setType(GizmoType::Scale);
                break;

            default:
                break;
            }

            break;
        }

        default:
            break;
        }
    }

    bool Gizmo::makeRay(const Chicane::Vec2& inLocation, Chicane::Vec3& outOrigin, Chicane::Vec3& outDirection) const
    {
        std::shared_ptr<Chicane::Grid::View> view = Chicane::Application::getInstance().getView();
        if (!view)
        {
            return false;
        }

        Chicane::Grid::Component* viewport = nullptr;
        for (Chicane::Grid::Component* child : view->getChildrenFlat())
        {
            if (!child || !child->getTag().equals(Chicane::Grid::Viewport::TAG_ID))
            {
                continue;
            }

            viewport = child;

            break;
        }

        if (!viewport)
        {
            return false;
        }

        Chicane::Scene* scene = getScene();
        if (!scene)
        {
            return false;
        }

        const Chicane::Vec2 size = viewport->getSize();
        const Chicane::Vec2 local(
            inLocation.x - viewport->getPosition().x,
            inLocation.y - viewport->getPosition().y
        );

        std::vector<Chicane::CCamera*> cameras = scene->getActiveComponents<Chicane::CCamera>();
        if (cameras.empty())
        {
            return false;
        }

        Chicane::CCamera*    camera = cameras.back();
        const Chicane::View& data   = camera->getData();
        Chicane::Vec3        nearPoint;
        Chicane::Vec3        farPoint;
        if (!Chicane::Mat4::fromPosition(local, data.view, data.projection, size, nearPoint, farPoint))
        {
            return false;
        }

        outOrigin                 = camera->getTranslation();
        const Chicane::Vec3 delta = farPoint - outOrigin;
        if (delta.dot(delta) < 0.0001f)
        {
            return false;
        }

        outDirection = delta.normalize();

        return true;
    }

    bool Gizmo::isOverViewport(const Chicane::Vec2& inLocation) const
    {
        std::shared_ptr<Chicane::Grid::View> view = Chicane::Application::getInstance().getView();
        if (!view)
        {
            return false;
        }

        Chicane::Grid::Component* hit = view->getHitAt(inLocation);
        while (hit)
        {
            if (hit->getTag().equals(Chicane::Grid::Viewport::TAG_ID))
            {
                return true;
            }

            if (hit->isRoot() || !hit->hasParent())
            {
                break;
            }

            hit = hit->getParent();
        }

        return false;
    }

    void Gizmo::pickAt(const Chicane::Vec2& inLocation)
    {
        if (!isOverViewport(inLocation))
        {
            return;
        }

        Chicane::Vec3 origin;
        Chicane::Vec3 direction;
        if (!makeRay(inLocation, origin, direction))
        {
            return;
        }

        if (pick(origin, direction))
        {
            return;
        }

        Chicane::Scene* scene = getScene();
        if (!scene)
        {
            return;
        }

        const Chicane::Vec3 destination = origin + direction * 1000.0f;
        const std::vector<Item*> hits   = scene->traceLine<Item>(origin, destination, {});
        Chicane::Object*         target = hits.empty() ? nullptr : hits.front();

        if (std::shared_ptr<HomeView> home = Chicane::Application::getInstance().getView<HomeView>())
        {
            home->onItemSelection(target);

            return;
        }

        setTarget(target);
    }

    void Gizmo::dragAt(const Chicane::Vec2& inLocation)
    {
        Chicane::Vec3 origin;
        Chicane::Vec3 direction;
        if (!makeRay(inLocation, origin, direction))
        {
            return;
        }

        drag(origin, direction);
    }
}
