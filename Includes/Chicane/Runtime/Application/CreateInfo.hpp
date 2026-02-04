#pragma once

#include "Chicane/Core/Window/Settings.hpp"

#include "Chicane/Renderer/Settings.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationCreateInfo
    {
    public:
        // Window
        WindowSettings        window = {};

        // Renderer
        Renderer::Settings    renderer = {};

        // Callbacks
        std::function<void()> onSetup = nullptr;
    };
}