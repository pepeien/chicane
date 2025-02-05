#include "Actor/Character.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

static constexpr float MOVE_COEFFICIENT = 3.0f;

Character::Character()
    : Chicane::ACharacter(),
    m_camera(nullptr),
    m_wand(nullptr),
    m_body(nullptr),
    m_hitAudio(nullptr),
    m_victoryAudio(nullptr)
{
    m_camera = Chicane::Application::getLevel()->createComponent<Chicane::CCamera>();
    m_camera->attachTo(this);
    m_camera->setRelativeTranslation(0.0f, 0.0f, 15.0f);
    m_camera->activate();

    m_wand = Chicane::Application::getLevel()->createComponent<Chicane::CMesh>();
    m_wand->attachTo(m_camera);
    m_wand->setMesh("Content/Sample/Meshes/Cube.bmsh");
    m_wand->setRelativeTranslation(0.15f, 0.4f, -0.1f);
    m_wand->setRelativeRotation(0.0f, 0.0f, 8.0f);
    m_wand->setRelativeScale(0.015f, 0.2f, 0.015f);
    m_wand->activate();

    m_body = Chicane::Application::getLevel()->createComponent<Chicane::CMesh>();
    m_body->attachTo(this);
    m_body->setMesh("Content/Sample/Meshes/Cube.bmsh");
    m_body->setRelativeTranslation(0.0f, -3.0f, 0.0f);
    m_body->setRelativeScale(1.0f, 1.0f, 10.0f);
    m_body->activate();

    m_hitAudio = Chicane::Application::getLevel()->createComponent<Chicane::CAudio>();
    m_hitAudio->attachTo(this);
    m_hitAudio->load("Content/Sample/Sounds/Hit.baud");
    m_hitAudio->activate();

    m_victoryAudio = Chicane::Application::getLevel()->createComponent<Chicane::CAudio>();
    m_victoryAudio->attachTo(this);
    m_victoryAudio->load("Content/Sample/Sounds/Victory.baud");
    m_victoryAudio->activate();

    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            if (Game::didReachMaxScore())
            {
                m_victoryAudio->play();

                return;
            }

            m_hitAudio->play();
        }
    );
}

void Character::onControlAttachment()
{
    m_controller->bindEvent(std::bind(&Character::onLook, this, std::placeholders::_1));

    m_controller->bindEvent(
        Chicane::Controller::MouseButton::Left,
        Chicane::Controller::EventStatus::Pressed,
        std::bind(&Character::onLeftClick, this)
    );

    m_controller->bindEvent(
        Chicane::Controller::KeyboardKey::W,
        Chicane::Controller::EventStatus::Pressed,
        std::bind(&Character::onMoveForward, this)
    );
    m_controller->bindEvent(
        Chicane::Controller::KeyboardKey::S,
        Chicane::Controller::EventStatus::Pressed,
        std::bind(&Character::onMoveBackward, this)
    );
    m_controller->bindEvent(
        Chicane::Controller::KeyboardKey::A,
        Chicane::Controller::EventStatus::Pressed,
        std::bind(&Character::onMoveLeft, this)
    );
    m_controller->bindEvent(
        Chicane::Controller::KeyboardKey::D,
        Chicane::Controller::EventStatus::Pressed,
        std::bind(&Character::onMoveRight, this)
    );
    m_controller->bindEvent(
        Chicane::Controller::KeyboardKey::Space,
        Chicane::Controller::EventStatus::Pressed,
        std::bind(&Chicane::ACharacter::jump, this)
    );
}

void Character::onLook(const Chicane::Controller::MouseMotionEvent& inEvent)
{
    m_camera->addRelativeRotation(-inEvent.yrel * 0.5f, 0.0f, 0.0f);
    addYaw(-inEvent.xrel * 0.5f);
}

void Character::onLeftClick()
{
    const Chicane::Vec<3, float>& origin     = m_camera->getTranslation();
    const Chicane::Vec<3, float> destination = origin + (m_camera->getForward() * m_camera->getFarClip());

    std::vector<Apple*> hitApples = Chicane::Application::getLevel()->traceLine<Apple>(
        origin,
        destination,
        { this }
    );
 
    for (Apple* apple : hitApples)
    {
        apple->onHit(this);
    }
}

void Character::onMoveForward()
{
    move(getForward(), MOVE_COEFFICIENT);
}

void Character::onMoveBackward()
{
    move(getForward(), -MOVE_COEFFICIENT);
}

void Character::onMoveLeft()
{
    move(getRight(), -MOVE_COEFFICIENT);
}

void Character::onMoveRight()
{
    move(getRight(), MOVE_COEFFICIENT);
}