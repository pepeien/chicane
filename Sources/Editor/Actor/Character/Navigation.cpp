#include "Editor/Actor/Character/Navigation.hpp"

#include <algorithm>
#include <cmath>

#include <Chicane/Grid/Component.hpp>
#include <Chicane/Grid/Component/Viewport.hpp>
#include <Chicane/Runtime/Application.hpp>

namespace Editor
{
    static constexpr inline const float ORBIT_SPEED     = 0.4f;
    static constexpr inline const float PAN_SPEED       = 0.0025f;
    static constexpr inline const float ZOOM_DRAG_SPEED = 0.01f;
    static constexpr inline const float ZOOM_WHEEL      = 0.85f;
    static constexpr inline const float MIN_DISTANCE    = 0.25f;
    static constexpr inline const float MAX_DISTANCE    = 100000.0f;
    static constexpr inline const float MIN_PITCH       = -89.0f;
    static constexpr inline const float MAX_PITCH       = 89.0f;

    Navigation::Navigation()
        : m_character(nullptr),
          m_camera(nullptr),
          m_yaw(45.0f),
          m_pitch(-35.0f),
          m_distance(17.32f),
          m_pivot(Chicane::Vec3::Zero()),
          m_bShift(false),
          m_bCtrl(false),
          m_bAlt(false),
          m_bMiddle(false),
          m_bLeft(false),
          m_bRight(false),
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
            m_bLeft = bInIsHeld;

            break;

        case Chicane::Input::MouseButton::Middle:
            m_bMiddle = bInIsHeld;

            break;

        case Chicane::Input::MouseButton::Right:
            m_bRight = bInIsHeld;

            break;

        default:
            return;
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
        switch (inButton)
        {
        case Chicane::Input::KeyboardButton::LShift:
        case Chicane::Input::KeyboardButton::RShift:
            m_bShift = bInIsHeld;

            break;

        case Chicane::Input::KeyboardButton::LCtrl:
        case Chicane::Input::KeyboardButton::RCtrl:
            m_bCtrl = bInIsHeld;

            break;

        case Chicane::Input::KeyboardButton::LAlt:
        case Chicane::Input::KeyboardButton::RAlt:
            m_bAlt = bInIsHeld;

            break;

        default:
            break;
        }
    }

    void Navigation::apply()
    {
        if (!m_character || !m_camera)
        {
            return;
        }

        m_pitch    = std::clamp(m_pitch, MIN_PITCH, MAX_PITCH);
        m_distance = std::clamp(m_distance, MIN_DISTANCE, MAX_DISTANCE);

        m_character->setAbsoluteRotation(0.0f, 0.0f, m_yaw);
        m_camera->setRelativeRotation(m_pitch, 0.0f, 0.0f);

        const Chicane::Vec3 forward = m_camera->getForward().normalize();
        m_character->setAbsoluteTranslation(m_pivot - forward * m_distance);
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

        const float         scale = m_distance * PAN_SPEED;
        const Chicane::Vec3 delta =
            m_camera->getRight().normalize() * (-inX * scale) + m_camera->getUp().normalize() * (inY * scale);
        m_pivot += delta;
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

        if (m_bMiddle)
        {
            if (m_bShift)
            {
                m_type = NavigationType::Pan;

                return;
            }

            if (m_bCtrl)
            {
                m_type = NavigationType::Zoom;

                return;
            }

            m_type = NavigationType::Orbit;

            return;
        }

        if (!m_bAlt)
        {
            return;
        }

        if (m_bLeft)
        {
            m_type = m_bShift ? NavigationType::Pan : NavigationType::Orbit;

            return;
        }

        if (m_bRight)
        {
            m_type = NavigationType::Zoom;
        }
    }

    void Navigation::stop()
    {
        if (m_bMiddle || (m_bAlt && (m_bLeft || m_bRight)))
        {
            return;
        }

        m_type = NavigationType::None;
    }

    bool Navigation::isViewportHovered() const
    {
        std::shared_ptr<Chicane::Grid::View> view = Chicane::Application::getInstance().getView();
        if (!view)
        {
            return true;
        }

        for (Chicane::Grid::Component* child : view->getChildrenFlat())
        {
            if (child && child->getTag().equals(Chicane::Grid::Viewport::TAG_ID) && child->isHovered())
            {
                return true;
            }
        }

        return false;
    }
}
