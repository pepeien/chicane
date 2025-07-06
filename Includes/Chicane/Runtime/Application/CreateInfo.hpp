#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationCreateInfo
    {
    public:
        // Window
        WindowCreateInfo       window   = {};

        // Callbacks
        std::function<void ()> onSetup  = nullptr;
    };
}