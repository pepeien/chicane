#include "Actor/Character.hpp"

#include <Chicane/Runtime/Application.hpp>

#include "Actor/Apple.hpp"
#include "Game.hpp"
#include "Level.hpp"

static constexpr inline const float MOVE_COEFFICIENT = 3.0f;

Character::Character()
    : Chicane::ACharacter(),
      m_camera(nullptr),
      m_wand(nullptr),
      m_body(nullptr),
      m_victorySound(nullptr)
{
    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            if (Game::didReachMaxScore())
            {
                m_victorySound->play();

                return;
            }
        }
    );
}

void Character::onLoad()
{
    Chicane::ACharacter::onLoad();

    m_camera = getScene()->createComponent<Chicane::CCamera>();
    m_camera->setId("First Person");
    m_camera->attachTo(this);
    m_camera->setRelativeTranslation(0.0f, 0.0f, 15.0f);
    m_camera->activate();

    m_wand = getScene()->createComponent<Chicane::CMesh>();
    m_wand->attachTo(m_camera);
    m_wand->setMesh("Contents/Sample/Shooter/Meshes/Character/Wand.bmsh");
    m_wand->setRelativeTranslation(0.15f, 0.4f, -0.1f);
    m_wand->setRelativeRotation(0.0f, 0.0f, 8.0f);
    m_wand->setRelativeScale(0.015f, 0.2f, 0.015f);
    m_wand->activate();

    m_body = getScene()->createComponent<Chicane::CMesh>();
    m_body->attachTo(this);
    m_body->setMesh("Contents/Sample/Shooter/Meshes/Character/Body.bmsh");
    m_body->activate();

    m_victorySound = getScene()->createComponent<Chicane::CSound>();
    m_victorySound->attachTo(this);
    m_victorySound->load("Contents/Sample/Shooter/Sounds/Victory.bsnd");
    m_victorySound->activate();
}

void Character::onControlAttachment()
{
    // Mouse
    m_controller->bindEvent(std::bind(&Character::onMouseMotion, this, std::placeholders::_1));

    m_controller->bindEvent(
        Chicane::Input::MouseButton::Left,
        Chicane::Input::Status::Pressed,
        std::bind(&Character::onLeftClick, this)
    );
    m_controller->bindEvent(
        Chicane::Input::MouseButton::Right,
        Chicane::Input::Status::Released,
        std::bind(&Character::onRightClick, this)
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
        std::bind(&Character::onJump, this)
    );

    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F1,
        Chicane::Input::Status::Pressed,
        [this]()
        {
            m_camera->activate();

            getScene<Level>()->disableCameras();
        }
    );
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F2,
        Chicane::Input::Status::Pressed,
        [this]()
        {
            getScene<Level>()->activateLeftCamera();

            m_camera->deactivate();
        }
    );
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F3,
        Chicane::Input::Status::Pressed,
        [this]()
        {
            getScene<Level>()->activateCenterCamera();

            m_camera->deactivate();
        }
    );
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F4,
        Chicane::Input::Status::Pressed,
        [this]() { getScene<Level>()->activateRightCamera(); }
    );

    // Gamepad
    m_controller->bindEvent(std::bind(&Character::onGamepadMotion, this, std::placeholders::_1));

    m_controller->bindEvent(
        Chicane::Input::GamepadButton::South,
        Chicane::Input::Status::Pressed,
        std::bind(&Character::onJump, this)
    );
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

    case Chicane::Input::GamepadAxis::RightTrigger:
        onShoot();

        break;

    default:
        break;
    }
}

void Character::onLeftClick()
{
    if (!Chicane::Application::getInstance().getWindow()->isFocused())
    {
        return;
    }

    onShoot();
}

void Character::onRightClick()
{
    Chicane::Application::getInstance().getWindow()->switchFocus();
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

void Character::onShoot()
{
    if (!Chicane::Application::getInstance().getWindow()->isFocused())
    {
        return;
    }

    const Chicane::Vec3& origin      = m_camera->getTranslation();
    const Chicane::Vec3  destination = origin + (m_camera->getForward() * m_camera->getFarClip());

    std::vector<Apple*> hitApples = getScene()->traceLine<Apple>(origin, destination, {this});

    for (Apple* apple : hitApples)
    {
        apple->onHit(this);
    }
}

void Character::onLook(float inX, float inY)
{
    if (!Chicane::Application::getInstance().getWindow()->isFocused())
    {
        return;
    }

    m_camera->addRelativeRotation(inY, 0.0f, 0.0f);

    addYaw(inX);
}

void Character::onJump()
{
    if (!Chicane::Application::getInstance().getWindow()->isFocused())
    {
        return;
    }

    jump();
}