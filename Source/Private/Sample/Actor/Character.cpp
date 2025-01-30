#include "Actor/Character.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

Character::Character()
    : Chicane::ACharacter(),\
    m_camera(new Chicane::CCamera()),
    m_wand(new Chicane::CMesh("Content/Sample/Meshes/Cube.bmsh")),
    m_hitAudio(new Chicane::CAudio("Content/Sample/Sounds/Hit.baud")),
    m_victoryAudio(new Chicane::CAudio("Content/Sample/Sounds/Victory.baud"))
{
    m_camera->attachTo(this);
    m_camera->activate();

    m_wand->attachTo(this);
    m_wand->setRelativeTranslation(0.15f, 0.4f, -0.1f);
    m_wand->setRelativeRotation(0.0f, 0.0f, 8.0f);
    m_wand->setRelativeScale(0.015f, 0.2f, 0.015f);
    m_wand->activate();

    m_hitAudio->attachTo(this);
    m_hitAudio->activate();

    m_victoryAudio->attachTo(this);
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
    m_controller->bindMouseMotionEvent(std::bind(&Character::onLook, this, std::placeholders::_1));

    m_controller->bindMouseButtonEvent(SDL_BUTTON_LEFT, std::bind(&Character::onLeftClick, this, std::placeholders::_1));

    m_controller->bindKeyboardButtonEvent(SDL_SCANCODE_W, std::bind(&Character::onMoveForward,  this, std::placeholders::_1));
    m_controller->bindKeyboardButtonEvent(SDL_SCANCODE_S, std::bind(&Character::onMoveBackward, this, std::placeholders::_1));
    m_controller->bindKeyboardButtonEvent(SDL_SCANCODE_A, std::bind(&Character::onMoveLeft,     this, std::placeholders::_1));
    m_controller->bindKeyboardButtonEvent(SDL_SCANCODE_D, std::bind(&Character::onMoveRight,    this, std::placeholders::_1));
}

void Character::onLook(const SDL_MouseMotionEvent& inEvent)
{
    addPitch(-inEvent.yrel * 0.5f);
    addYaw(-inEvent.xrel * 0.5f);
}

void Character::onLeftClick(bool bInIsButtonPressed)
{
    if (!bInIsButtonPressed)
    {
        return;
    }

    const Chicane::Vec<3, float>& origin     = getTranslation();
    const Chicane::Vec<3, float> destination = origin + (getForward() * m_camera->getFarClip());

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

void Character::onMoveForward(bool bInIsButtonPressed)
{
    if (!bInIsButtonPressed)
    {
        return;
    }

    move(getForward(), 1.0f);
}

void Character::onMoveBackward(bool bInIsButtonPressed)
{
    if (!bInIsButtonPressed)
    {
        return;
    }

    move(getForward(), -1.0f);
}

void Character::onMoveLeft(bool bInIsButtonPressed)
{
    if (!bInIsButtonPressed)
    {
        return;
    }

    move(getRight(), -1.0f);
}

void Character::onMoveRight(bool bInIsButtonPressed)
{
    if (!bInIsButtonPressed)
    {
        return;
    }

    move(getRight(), 1.0f);
}