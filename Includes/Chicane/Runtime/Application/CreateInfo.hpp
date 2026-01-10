#pragma once

#include "Chicane/Core/Window/CreateInfo.hpp"
#include "Chicane/Runtime.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationCreateInfo
    {
    public:
        // Window
        WindowCreateInfo      window  = {};

        // Callbacks
        std::function<void()> onSetup = nullptr;
    };
}