#pragma once

#include "Chicane/Core/Window/Settings.hpp"

#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationCreateInfo
    {
    public:
        // Window
        WindowSettings        window = {};

        // Callbacks
        std::function<void()> onSetup = nullptr;
    };
}