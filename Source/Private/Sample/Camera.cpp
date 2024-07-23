#include "Camera.hpp"

#include "Chicane/Base.hpp"

Camera::Camera()
    : Chicane::Camera()
{
    setPosition(-250000.0f, 0.0f, 0.0f);
    setRotation(-120.0f, 0.0f, 0.0f);
}

void Camera::onEvent(const SDL_Event& inEvent)
{
    if (inEvent.type == SDL_MOUSEWHEEL)
    {
        zoom(
            5.0f * inEvent.wheel.y
        );
    }

    if (inEvent.type != SDL_MOUSEMOTION)
    {
        return;
    }

    switch (inEvent.motion.state)
    {
    case SDL_BUTTON_LMASK:
        addPitch(inEvent.motion.yrel);
        addYaw(inEvent.motion.xrel);

        break;
    case SDL_BUTTON_MMASK:
        zoom(inEvent.motion.yrel);

        break;

    case SDL_BUTTON_RMASK:
        pan(
            Chicane::Vec<float>::Two(
                inEvent.motion.xrel,
                inEvent.motion.yrel
            )
        );

        break;

    default:
        break;
    }
}