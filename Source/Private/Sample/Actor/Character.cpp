#include "Actor/Character.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Apple.hpp"

Character::Character()
    : Chicane::CameraPawn(),
    m_wand(new Chicane::MeshComponent())
{
    m_wand->setRelativeTranslation(0.15f, 0.4f, -0.1f);
    m_wand->setRelativeRotation(0.0f, 0.0f, 8.0f);
    m_wand->setRelativeScale(0.015f, 0.2f, 0.015f);
    m_wand->setMesh("Content/Sample/Meshes/Cube.bmsh");
    m_wand->attachTo(this);
    m_wand->activate();
}

void Character::onControlAttachment()
{
    Chicane::CameraPawn::onControlAttachment();

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