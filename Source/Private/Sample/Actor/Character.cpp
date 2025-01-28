#include "Actor/Character.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

Character::Character()
    : Chicane::ACameraPawn(),
    m_wand(new Chicane::CMesh("Content/Sample/Meshes/Cube.bmsh")),
    m_hitAudio(new Chicane::CAudio("Content/Sample/Sounds/Hit.baud")),
    m_victoryAudio(new Chicane::CAudio("Content/Sample/Sounds/Victory.baud"))
{
    m_wand->setRelativeTranslation(0.15f, 0.4f, -0.1f);
    m_wand->setRelativeRotation(0.0f, 0.0f, 8.0f);
    m_wand->setRelativeScale(0.015f, 0.2f, 0.015f);
    m_wand->attachTo(this);
    m_wand->activate();

    m_hitAudio->attachTo(this);
    m_victoryAudio->attachTo(this);

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
    Chicane::ACameraPawn::onControlAttachment();

    m_controller->bindMouseButtonEvent(
        SDL_BUTTON_LEFT,
        [this](bool bInIsButtonPressed)
        {
            if (!bInIsButtonPressed || !Chicane::Application::getWindow()->isFocused())
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
    );
}