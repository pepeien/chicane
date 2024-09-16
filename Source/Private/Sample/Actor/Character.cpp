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
            if (!bInIsButtonPressed)
            {
                return;
            }

            const Chicane::Vec<3, float>& direction  = getForward();
            const Chicane::Vec<3, float>& origin     = getTranslation();
            const Chicane::Vec<3, float> destination = origin + (direction * m_camera->getFarClip());

            std::vector<Chicane::Actor*> hitActors = Chicane::traceLine(
                origin,
                destination,
                direction,
                { this }
            );
 
            for (Chicane::Actor* actor : hitActors)
            {
                Chicane::Log::info(
                    "%p hit by %p",
                    actor, this
                );
            }
        }
    );
}