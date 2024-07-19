#pragma once

#include "Runtime/Renderer.hpp"

namespace Chicane
{
    namespace Editor
    {
        class ViewportCamera : public Camera
        {
            void onEvent(const SDL_Event& inEvent) override;
        };
    }
}