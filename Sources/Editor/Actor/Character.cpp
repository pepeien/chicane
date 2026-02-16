#include "Actor/Character.hpp"

#include <Chicane/Runtime/Application.hpp>
#include <Chicane/Runtime/Scene.hpp>

static constexpr inline const float MOVE_COEFFICIENT = 1.0f;

namespace Editor
{
    Character::Character()
        : Chicane::ACharacter(),
          m_camera(nullptr)
    {}

    void Character::onLoad()
    {
        Chicane::ACharacter::onLoad();

        m_camera = getScene()->createComponent<Chicane::CCamera>();
        m_camera->attachTo(this);
        m_camera->activate();
    }

    void Character::onControlAttachment()
    {
        // Mouse
        m_controller->bindEvent(std::bind(&Character::onMouseMotion, this, std::placeholders::_1));

        m_controller->bindEvent(
            Chicane::Input::MouseButton::Left,
            Chicane::Input::Status::Released,
            []() { Chicane::Application::getInstance().getWindow()->focus(); }
        );
        m_controller->bindEvent(
            Chicane::Input::MouseButton::Right,
            Chicane::Input::Status::Released,
            []() { Chicane::Application::getInstance().getWindow()->blur(); }
        );

        // Keyboard
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::W,
            Chicane::Input::Status::Pressed,
            std::bind(&Character::onMoveForward, this)
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::S,
            Chicane::Input::Status::Pressed,
            std::bind(&Character::onMoveBackward, this)
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::A,
            Chicane::Input::Status::Pressed,
            std::bind(&Character::onMoveLeft, this)
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::D,
            Chicane::Input::Status::Pressed,
            std::bind(&Character::onMoveRight, this)
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::Space,
            Chicane::Input::Status::Pressed,
            std::bind(&Character::onMoveUp, this)
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::LCtrl,
            Chicane::Input::Status::Pressed,
            std::bind(&Character::onMoveDown, this)
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::F1,
            Chicane::Input::Status::Released,
            []() { Chicane::Application::getInstance().setRenderer(Chicane::WindowBackend::Vulkan); }
        );
        m_controller->bindEvent(
            Chicane::Input::KeyboardButton::F2,
            Chicane::Input::Status::Released,
            []() { Chicane::Application::getInstance().setRenderer(Chicane::WindowBackend::OpenGL); }
        );

        // Gamepad
        m_controller->bindEvent(std::bind(&Character::onGamepadMotion, this, std::placeholders::_1));
    }

    void Character::look(float inX, float inY)
    {
        m_camera->addRelativeRotation(inY, 0.0f, 0.0f);
        addAbsoluteRotation(0.0f, 0.0f, inX);
    }

    void Character::onMouseMotion(const Chicane::Input::MouseMotionEvent& inEvent)
    {
        onLook(-inEvent.relativeLocation.x * 0.5f, -inEvent.relativeLocation.y * 0.5f);
    }

    void Character::onGamepadMotion(const Chicane::Input::GamepadMotionEvent& inEvent)
    {
        switch (inEvent.axis)
        {
        case Chicane::Input::GamepadAxis::LeftX:
        case Chicane::Input::GamepadAxis::LeftY:
            if (inEvent.axis == Chicane::Input::GamepadAxis::LeftY)
            {
                if (inEvent.value < 0.0f)
                {
                    onMoveForward();
                }
                else
                {
                    onMoveBackward();
                }
            }
            else
            {
                if (inEvent.value > 0.0f)
                {
                    onMoveRight();
                }
                else
                {
                    onMoveLeft();
                }
            }

            break;

        case Chicane::Input::GamepadAxis::RightX:
        case Chicane::Input::GamepadAxis::RightY:
            if (inEvent.axis == Chicane::Input::GamepadAxis::RightY)
            {
                onLook(0.0f, -inEvent.value);
            }
            else
            {
                onLook(-inEvent.value, 0.0f);
            }

            break;

        default:
            break;
        }
    }

    void Character::onMoveForward()
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        move(getForward(), MOVE_COEFFICIENT);
    }

    void Character::onMoveBackward()
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        move(getForward(), -MOVE_COEFFICIENT);
    }

    void Character::onMoveLeft()
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        move(getRight(), -MOVE_COEFFICIENT);
    }

    void Character::onMoveRight()
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        move(getRight(), MOVE_COEFFICIENT);
    }

    void Character::onLook(float inX, float inY)
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        look(inX, inY);
    }

    void Character::onMoveUp()
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        move(getUp(), MOVE_COEFFICIENT);
    }

    void Character::onMoveDown()
    {
        if (!Chicane::Application::getInstance().getWindow()->isFocused())
        {
            return;
        }

        move(getUp(), -MOVE_COEFFICIENT);
    }
}