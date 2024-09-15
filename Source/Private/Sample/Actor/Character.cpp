#include "Actor/Character.hpp"

#include "Chicane/Core.hpp"

#include <iostream>

Character::Character()
    : Chicane::CameraActor()
{}

void Character::onControlAttachment()
{
    Chicane::CameraActor::onControlAttachment();

    m_controller->bindMouseButtonEvent(
        SDL_BUTTON_LEFT,
        [this](bool bInIsButtonPressed)
        {
            if (!bInIsButtonPressed)
            {
                return;
            }

            const Chicane::Vec<3, float>& forward    = getForward();
            const Chicane::Vec<3, float>& origin     = getTranslation();
            const Chicane::Vec<3, float> destination = origin + (forward * m_camera->getFarClip());

            std::vector<Chicane::Actor*> hitActors = Chicane::traceLine(
                origin,
                destination,
                forward,
                { this }
            );

            for (Chicane::Actor* actor : hitActors)
            {
                std::cout << "HIT => " << actor << "\n";
            }
        }
    );
}