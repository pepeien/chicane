#include "Actor/Character.hpp"

#include "Chicane/Core.hpp"
#include "Apple.hpp"

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
            if (!bInIsButtonPressed || !Chicane::isWindowFocused())
            {
                return;
            }

            const Chicane::Vec<3, float>& origin     = getTranslation();
            const Chicane::Vec<3, float> destination = origin + (getForward() * m_camera->getFarClip());

            std::vector<Chicane::Actor*> hitActors = Chicane::traceLine(
                origin,
                destination,
                { this }
            );
 
            for (Chicane::Actor* actor : hitActors)
            {
                actor->collideWith(this);
            }
        }
    );
}