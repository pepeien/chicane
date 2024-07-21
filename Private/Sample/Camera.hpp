#pragma once

#include "Chicane/Renderer.hpp"

class Camera : public Chicane::Camera
{
    void onEvent(const SDL_Event& inEvent) override;
};