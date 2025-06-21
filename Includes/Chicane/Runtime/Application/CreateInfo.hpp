#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer.hpp"

namespace Chicane
{
    struct CHICANE_RUNTIME ApplicationCreateInfo
    {
    public:
        // Renderer
        RendererCreateInfo     renderer = {};

        // Window
        WindowCreateInfo       window   = {};

        // Callbacks
        std::function<void ()> onSetup  = nullptr;
    };
}