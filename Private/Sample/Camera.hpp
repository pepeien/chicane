#pragma once

#include "Chicane/Renderer.hpp"

class Camera : public Chicane::Camera
{
public:
    Camera();

public:
    void onEvent(const SDL_Event& inEvent) override;
};