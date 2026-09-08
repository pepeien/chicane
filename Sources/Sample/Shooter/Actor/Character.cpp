#include "Sample/Shooter/Actor/Character.hpp"

#include <cmath>

#include <Chicane/Runtime/Application.hpp>

#include "Sample/Shooter/Actor/Apple.hpp"
#include "Sample/Shooter/Game.hpp"
#include "Sample/Shooter/Scene.hpp"

constexpr inline float MOVE_COEFFICIENT = 3.0f;
constexpr inline float JUMP_SPEED       = 45.0f;
constexpr inline float GAMEPAD_DEADZONE = 0.3f;
constexpr inline float LOOK_MOUSE_SCALE = 0.5f;

Character::Character()
    : Chicane::ACharacter(),
      m_bIsMovingForward(false),
      m_bIsMovingBackward(false),
      m_bIsMovingLeft(false),
      m_bIsMovingRight(false),
      m_padForward(0.0f),
      m_padRight(0.0f),
      m_camera(nullptr),
      m_wand(nullptr),
      m_body(nullptr),
      m_victorySound(nullptr)
{}

void Character::onLoad()
{
    Chicane::ACharacter::onLoad();

    setMoveScale(MOVE_COEFFICIENT);
    setAbsoluteTranslation(Chicane::Vec3(0.0f, -150.0f, 0.0f));

    m_camera = getScene()->createComponent<Chicane::CCamera>();
    m_camera->setId("First Person");
    m_camera->attachTo(this);
    m_camera->setRelativeTranslation(0.0f, 0.0f, 15.0f);
    m_camera->activate();

    m_wand = getScene()->createComponent<Chicane::CMesh>();
    m_wand->attachTo(m_camera);
    m_wand->setMesh("Assets/Sample/Shooter/Meshes/Character/Wand.bmsh");
    m_wand->setRelativeTranslation(0.15f, 0.4f, -0.1f);
    m_wand->setRelativeRotation(0.0f, 0.0f, 8.0f);
    m_wand->setRelativeScale(0.015f, 0.2f, 0.015f);
    m_wand->activate();

    m_body = getScene()->createComponent<Chicane::CMesh>();
    m_body->attachTo(this);
    m_body->setMesh("Assets/Sample/Shooter/Meshes/Character/Body.bmsh");
    m_body->activate();

    enablePhysics();

    m_victorySound = getScene()->createComponent<Chicane::CSound>();
    m_victorySound->attachTo(this);
    m_victorySound->load("Assets/Sample/Shooter/Sounds/Victory.bsnd");
    m_victorySound->activate();

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

void Character::onInput()
{
    refreshMoveInput();
    Chicane::ACharacter::onInput();
}

void Character::onControlAttachment()
{
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

    const auto bindMoveKey = [this](Chicane::Input::KeyboardButton inButton)
    {
        m_controller
            ->bindEvent(inButton, Chicane::Input::Status::Pressed, [this, inButton]() { onMoveKey(inButton, true); });
        m_controller
            ->bindEvent(inButton, Chicane::Input::Status::Released, [this, inButton]() { onMoveKey(inButton, false); });
    };

    bindMoveKey(Chicane::Input::KeyboardButton::W);
    bindMoveKey(Chicane::Input::KeyboardButton::S);
    bindMoveKey(Chicane::Input::KeyboardButton::A);
    bindMoveKey(Chicane::Input::KeyboardButton::D);
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::Space,
        Chicane::Input::Status::Pressed,
        std::bind(&Character::onJump, this)
    );

    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F1,
        Chicane::Input::Status::Released,
        [this]()
        {
            m_camera->activate();

            getScene<Scene>()->disableCameras();
        }
    );
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F2,
        Chicane::Input::Status::Released,
        [this]()
        {
            getScene<Scene>()->activateLeftCamera();

            m_camera->deactivate();
        }
    );
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F3,
        Chicane::Input::Status::Released,
        [this]()
        {
            getScene<Scene>()->activateCenterCamera();

            m_camera->deactivate();
        }
    );
    m_controller->bindEvent(
        Chicane::Input::KeyboardButton::F4,
        Chicane::Input::Status::Released,
        [this]() { getScene<Scene>()->activateRightCamera(); }
    );

    m_controller->bindEvent(std::bind(&Character::onGamepadMotion, this, std::placeholders::_1));

    m_controller->bindEvent(
        Chicane::Input::GamepadButton::South,
        Chicane::Input::Status::Pressed,
        std::bind(&Character::onJump, this)
    );

    m_controller->bindEvent(
        Chicane::Input::GamepadButton::LeftShoulder,
        Chicane::Input::Status::Pressed,
        std::bind(&Character::onShoot, this)
    );

    m_controller->bindEvent(
        Chicane::Input::GamepadButton::Start,
        Chicane::Input::Status::Released,
        std::bind(&Character::onRightClick, this)
    );
}

void Character::onMouseMotion(const Chicane::Input::MouseMotionEvent& inEvent)
{
    onLook(-inEvent.relativeLocation.x * LOOK_MOUSE_SCALE, -inEvent.relativeLocation.y * LOOK_MOUSE_SCALE);
}

void Character::onGamepadMotion(const Chicane::Input::GamepadMotionEvent& inEvent)
{
    switch (inEvent.axis)
    {
    case Chicane::Input::GamepadAxis::LeftX:
    case Chicane::Input::GamepadAxis::LeftY: {
        const float value = std::abs(inEvent.value) <= GAMEPAD_DEADZONE ? 0.0f : inEvent.value;
        if (inEvent.axis == Chicane::Input::GamepadAxis::LeftY)
        {
            m_padForward = -value;
        }
        else
        {
            m_padRight = value;
        }

        refreshMoveInput();

        break;
    }

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

void Character::onMoveKey(Chicane::Input::KeyboardButton inButton, bool inHeld)
{
    switch (inButton)
    {
    case Chicane::Input::KeyboardButton::W:
        m_bIsMovingForward = inHeld;
        break;
    case Chicane::Input::KeyboardButton::S:
        m_bIsMovingBackward = inHeld;
        break;
    case Chicane::Input::KeyboardButton::A:
        m_bIsMovingLeft = inHeld;
        break;
    case Chicane::Input::KeyboardButton::D:
        m_bIsMovingRight = inHeld;
        break;
    default:
        return;
    }

    refreshMoveInput();
}

void Character::refreshMoveInput()
{
    if (!Chicane::Application::getInstance().getWindow()->isFocused())
    {
        setMoveInput(0.0f, 0.0f);

        return;
    }

    const float forward = (m_bIsMovingForward ? 1.0f : 0.0f) - (m_bIsMovingBackward ? 1.0f : 0.0f) + m_padForward;
    const float right   = (m_bIsMovingRight ? 1.0f : 0.0f) - (m_bIsMovingLeft ? 1.0f : 0.0f) + m_padRight;

    setMoveInput(forward, right);
}

void Character::onShoot()
{
    if (!Chicane::Application::getInstance().getWindow()->isFocused())
    {
        return;
    }

    const Chicane::Vec3& origin      = m_camera->getTranslation();
    const Chicane::Vec3  destination = origin + (m_camera->getForward() * m_camera->getFarClip());

    std::vector<Chicane::SceneTraceResponse> hits;
    if (!getScene()->traceMulti<Apple>(
            hits,
            Chicane::SceneTraceRequest::Line(origin, destination),
            {this}
        ))
    {
        return;
    }

    for (const Chicane::SceneTraceResponse& hit : hits)
    {
        if (Apple* apple = static_cast<Apple*>(hit.actor))
        {
            apple->onHit(this);
        }
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

    jump(JUMP_SPEED);
}
