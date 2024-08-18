#include "Camera.hpp"

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"

Camera::Camera()
    : Chicane::Camera()
{
    setPosition(-250000.0f, 0.0f, 0.0f);
    setRotation(-120.0f, 0.0f, 0.0f);
}

void Camera::onEvent(const SDL_Event& inEvent)
{
    if (inEvent.type == SDL_MOUSEBUTTONDOWN)
    {
        if (inEvent.button.button == SDL_BUTTON_LEFT)
        {
            Chicane::showCursor(!Chicane::isShowingCursor());
        }
    }

    if (Chicane::isShowingCursor())
    {
        return;
    }

    switch (inEvent.type)
    {
    case SDL_MOUSEMOTION:
        addPitch(inEvent.motion.yrel);
        addYaw(inEvent.motion.xrel);

        break;

    default:
        break;
    }
}