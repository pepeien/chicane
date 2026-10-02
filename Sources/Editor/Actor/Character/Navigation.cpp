#include "Editor/Actor/Character/Navigation.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_set>

#include <Chicane/Box/Asset/Preview.hpp>
#include <Chicane/Core/Math.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/Viewport.hpp>
#include <Chicane/Runtime/Instance.hpp>

#include "Editor/Scene.hpp"

namespace Editor
{
    static void viewBasis(
        float inYaw, float inPitch, Chicane::Vec3& outRight, Chicane::Vec3& outUp, Chicane::Vec3& outForward
    )
    {
        const Chicane::QuatFloat orientation =
            Chicane::Rotator(0.0f, 0.0f, inYaw).get() * Chicane::Rotator(inPitch, 0.0f, 0.0f).get();
        const Chicane::Rotator axes(orientation);

        outRight   = axes.getRight().normalize();
        outUp      = axes.getUp().normalize();
        outForward = axes.getForward().normalize();
    }

    static constexpr std::uint8_t buttonBit(Chicane::Input::MouseButton inButton)
    {
        return static_cast<std::uint8_t>(1u << (static_cast<std::uint8_t>(inButton) - 1u));
    }

    static Chicane::Input::KeyboardButtonModifier modifierFrom(Chicane::Input::KeyboardButton inButton)
    {
        switch (inButton)
        {
        case Chicane::Input::KeyboardButton::LShift:
            return Chicane::Input::KeyboardButtonModifier::LeftShift;

        case Chicane::Input::KeyboardButton::RShift:
            return Chicane::Input::KeyboardButtonModifier::RightShift;

        case Chicane::Input::KeyboardButton::LCtrl:
            return Chicane::Input::KeyboardButtonModifier::LeftCtrl;

        case Chicane::Input::KeyboardButton::RCtrl:
            return Chicane::Input::KeyboardButtonModifier::RightCtrl;

        case Chicane::Input::KeyboardButton::LAlt:
            return Chicane::Input::KeyboardButtonModifier::LeftAlt;

        case Chicane::Input::KeyboardButton::RAlt:
            return Chicane::Input::KeyboardButtonModifier::RightAlt;

        default:
            return Chicane::Input::KeyboardButtonModifier::None;
        }
    }

    Navigation::Navigation()
        : m_character(nullptr),
          m_camera(nullptr),
          m_yaw(32.8f),
          m_pitch(-40.0f),
          m_distance(Chicane::Box::AssetPreview::START_DISTANCE),
          m_pivot(Chicane::Vec3(0.0f, 0.0f, 0.45f)),
          m_modifiers(Chicane::Input::KeyboardButtonModifier::None),
          m_buttons(0),
          m_type(NavigationType::None)
    {}

    void Navigation::attach(Chicane::ACharacter* inCharacter, Chicane::CCamera* inCamera)
    {
        m_character = inCharacter;
        m_camera    = inCamera;

        if (!m_character)
        {
            return;
        }

        const Chicane::Vec3 offset = m_character->getAbsoluteTranslation() - m_pivot;
        m_distance                 = std::sqrt(offset.dot(offset));
        if (m_distance < MIN_DISTANCE)
        {
            m_distance = MIN_DISTANCE;
        }

        apply();
    }

    void Navigation::bind(Chicane::Controller* inController)
    {
        if (!inController)
        {
            return;
        }

        inController->bindEvent(std::bind(&Navigation::onMouseMotion, this, std::placeholders::_1));
        inController->bindEvent(std::bind(&Navigation::onMouseWheel, this, std::placeholders::_1));

        const auto bindMouse = [this, inController](Chicane::Input::MouseButton inButton)
        {
            inController->bindEvent(
                inButton,
                Chicane::Input::Status::Pressed,
                [this, inButton]() { onMouseButton(inButton, true); }
            );
            inController->bindEvent(
                inButton,
                Chicane::Input::Status::Released,
                [this, inButton]() { onMouseButton(inButton, false); }
            );
        };
        bindMouse(Chicane::Input::MouseButton::Left);
        bindMouse(Chicane::Input::MouseButton::Middle);
        bindMouse(Chicane::Input::MouseButton::Right);

        const auto bindModifier = [this, inController](Chicane::Input::KeyboardButton inButton)
        {
            inController->bindEvent(
                inButton,
                Chicane::Input::Status::Pressed,
                [this, inButton]() { onModifierKey(inButton, true); }
            );
            inController->bindEvent(
                inButton,
                Chicane::Input::Status::Released,
                [this, inButton]() { onModifierKey(inButton, false); }
            );
        };
        bindModifier(Chicane::Input::KeyboardButton::LShift);
        bindModifier(Chicane::Input::KeyboardButton::RShift);
        bindModifier(Chicane::Input::KeyboardButton::LCtrl);
        bindModifier(Chicane::Input::KeyboardButton::RCtrl);
        bindModifier(Chicane::Input::KeyboardButton::LAlt);
        bindModifier(Chicane::Input::KeyboardButton::RAlt);

        inController->bindEvent(std::bind(&Navigation::onGamepadMotion, this, std::placeholders::_1));
    }

    void Navigation::frame(const Chicane::Vec3& inPosition, const Chicane::Vec3& inPivot)
    {
        m_pivot                    = inPivot;
        const Chicane::Vec3 offset = inPosition - inPivot;
        const float         horiz  = std::sqrt(offset.x * offset.x + offset.y * offset.y);

        m_distance = std::sqrt(offset.dot(offset));
        if (m_distance < MIN_DISTANCE)
        {
            m_distance = MIN_DISTANCE;
        }

        m_yaw   = std::atan2(offset.x, -offset.y) * Chicane::Math::RAD_TO_DEG;
        m_pitch = -std::atan2(offset.z, std::max(horiz, 0.0001f)) * Chicane::Math::RAD_TO_DEG;

        apply();
    }

    void Navigation::onMouseMotion(const Chicane::Input::MouseMotionEvent& inEvent)
    {
        if (m_type == NavigationType::None)
        {
            return;
        }

        const float dx = inEvent.relativeLocation.x;
        const float dy = inEvent.relativeLocation.y;

        switch (m_type)
        {
        case NavigationType::Orbit:
            orbit(-dx * ORBIT_SPEED, -dy * ORBIT_SPEED);

            break;

        case NavigationType::Pan:
            pan(dx, dy);

            break;

        case NavigationType::Zoom:
            zoom(dy * ZOOM_DRAG_SPEED);

            break;

        default:
            break;
        }
    }

    void Navigation::onMouseWheel(const Chicane::Input::MouseWheelEvent& inEvent)
    {
        if (!isViewportHovered())
        {
            return;
        }

        zoom(-inEvent.delta.y);
    }

    void Navigation::onMouseButton(Chicane::Input::MouseButton inButton, bool bInIsHeld)
    {
        switch (inButton)
        {
        case Chicane::Input::MouseButton::Left:
        case Chicane::Input::MouseButton::Middle:
        case Chicane::Input::MouseButton::Right:
            break;

        default:
            return;
        }

        const std::uint8_t bit = buttonBit(inButton);
        if (bInIsHeld)
        {
            m_buttons |= bit;
        }
        else
        {
            m_buttons &= static_cast<std::uint8_t>(~bit);
        }

        if (bInIsHeld)
        {
            start();

            return;
        }

        stop();
    }

    void Navigation::onGamepadMotion(const Chicane::Input::GamepadMotionEvent& inEvent)
    {
        if (inEvent.axis != Chicane::Input::GamepadAxis::RightX && inEvent.axis != Chicane::Input::GamepadAxis::RightY)
        {
            return;
        }

        if (std::abs(inEvent.value) <= 0.3f)
        {
            return;
        }

        if (inEvent.axis == Chicane::Input::GamepadAxis::RightY)
        {
            orbit(0.0f, -inEvent.value);

            return;
        }

        orbit(-inEvent.value, 0.0f);
    }

    void Navigation::onModifierKey(Chicane::Input::KeyboardButton inButton, bool bInIsHeld)
    {
        const Chicane::Input::KeyboardButtonModifier flag = modifierFrom(inButton);
        if (flag == Chicane::Input::KeyboardButtonModifier::None)
        {
            return;
        }

        const std::uint16_t bits = static_cast<std::uint16_t>(m_modifiers);
        const std::uint16_t mask = static_cast<std::uint16_t>(flag);
        const std::uint16_t next = bInIsHeld ? (bits | mask) : (bits & static_cast<std::uint16_t>(~mask));
        m_modifiers              = static_cast<Chicane::Input::KeyboardButtonModifier>(next);
    }

    void Navigation::apply()
    {
        if (!m_character || !m_camera)
        {
            return;
        }

        m_pitch    = std::clamp(m_pitch, MIN_PITCH, MAX_PITCH);
        m_distance = std::clamp(m_distance, MIN_DISTANCE, MAX_DISTANCE);

        const float         yaw      = m_yaw;
        const float         pitch    = m_pitch;
        const float         distance = m_distance;
        const Chicane::Vec3 pivot    = m_pivot;

        Chicane::ACharacter* character = m_character;
        Chicane::CCamera*    camera    = m_camera;

        const auto commit = [character, camera, yaw, pitch, distance, pivot]()
        {
            Chicane::Vec3 right;
            Chicane::Vec3 up;
            Chicane::Vec3 forward;
            viewBasis(yaw, pitch, right, up, forward);

            character->setAbsoluteRotation(0.0f, 0.0f, yaw);
            camera->setRelativeRotation(pitch, 0.0f, 0.0f);
            character->setAbsoluteTranslation(pivot - forward * distance);
        };

        const std::shared_ptr<Chicane::Scene> scene = Chicane::Instance::sInstance().getScene();
        if (scene && !scene->ownsObjects())
        {
            scene->runOnOwner(commit);

            return;
        }

        commit();
    }

    void Navigation::orbit(float inYaw, float inPitch)
    {
        m_yaw += inYaw;
        m_pitch += inPitch;

        apply();
    }

    void Navigation::pan(float inX, float inY)
    {
        if (!m_camera)
        {
            return;
        }

        Chicane::Vec3 right   = Chicane::Vec3::sZero();
        Chicane::Vec3 up      = Chicane::Vec3::sZero();
        Chicane::Vec3 forward = Chicane::Vec3::sZero();
        viewBasis(m_yaw, m_pitch, right, up, forward);

        const float scale = m_distance * PAN_SPEED;
        m_pivot += right * (-inX * scale) + up * (inY * scale);

        apply();
    }

    void Navigation::zoom(float inDelta)
    {
        if (std::abs(inDelta) <= 0.0001f)
        {
            return;
        }

        m_distance *= std::pow(ZOOM_WHEEL, -inDelta);

        apply();
    }

    void Navigation::start()
    {
        if (m_type != NavigationType::None || !isViewportHovered())
        {
            return;
        }

        if (std::shared_ptr<Scene> scene = std::dynamic_pointer_cast<Scene>(Chicane::Instance::sInstance().getScene()))
        {
            if (Gizmo* gizmo = scene->getGizmo())
            {
                if (gizmo->isDragging() || (has(Chicane::Input::MouseButton::Left) && gizmo->isHandleHovered()))
                {
                    return;
                }
            }
        }

        if (has(Chicane::Input::MouseButton::Middle))
        {
            if (has(Chicane::Input::KeyboardButtonModifier::Shift))
            {
                m_type = NavigationType::Pan;

                return;
            }

            if (has(Chicane::Input::KeyboardButtonModifier::Ctrl))
            {
                m_type = NavigationType::Zoom;

                return;
            }

            m_type = NavigationType::Orbit;

            return;
        }

        if (!has(Chicane::Input::KeyboardButtonModifier::Alt))
        {
            return;
        }

        if (has(Chicane::Input::MouseButton::Left))
        {
            m_type = has(Chicane::Input::KeyboardButtonModifier::Shift) ? NavigationType::Pan : NavigationType::Orbit;

            return;
        }

        if (has(Chicane::Input::MouseButton::Right))
        {
            m_type = NavigationType::Zoom;
        }
    }

    void Navigation::stop()
    {
        const bool bMouseHeld = has(Chicane::Input::MouseButton::Middle) ||
                                (has(Chicane::Input::KeyboardButtonModifier::Alt) &&
                                 (has(Chicane::Input::MouseButton::Left) || has(Chicane::Input::MouseButton::Right)));
        if (bMouseHeld)
        {
            return;
        }

        m_type = NavigationType::None;
    }

    bool Navigation::has(Chicane::Input::KeyboardButtonModifier inModifier) const
    {
        const std::uint16_t bits = static_cast<std::uint16_t>(m_modifiers);
        const std::uint16_t mask = static_cast<std::uint16_t>(inModifier);

        return (bits & mask) != 0;
    }

    bool Navigation::has(Chicane::Input::MouseButton inButton) const
    {
        return (m_buttons & buttonBit(inButton)) != 0;
    }

    bool Navigation::isViewportHovered() const
    {
        std::shared_ptr<Chicane::Grid::View> view = Chicane::Instance::sInstance().getView();
        if (!view)
        {
            return true;
        }

        std::unordered_set<const Chicane::Grid::Component*> visited;
        visited.insert(view.get());

        std::vector<Chicane::Grid::Component*> stack;
        stack.push_back(view.get());

        while (!stack.empty())
        {
            Chicane::Grid::Component* node = stack.back();
            stack.pop_back();

            if (!node)
            {
                continue;
            }

            if (node->getTag().equals(Chicane::Grid::Viewport::TAG_ID) && node->isHovered())
            {
                return !node->getAttribute(Chicane::Grid::Component::ON_HOVER_ATTRIBUTE_NAME).isEmpty();
            }

            for (Chicane::Grid::Component* child : node->getChildren())
            {
                if (child && visited.insert(child).second)
                {
                    stack.push_back(child);
                }
            }
        }

        return false;
    }
}
